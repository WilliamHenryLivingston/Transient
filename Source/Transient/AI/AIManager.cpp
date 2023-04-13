// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIManager.h"

#include "Kismet/GameplayStatics.h"

#include "../UnitPawn.h"

AAIManager* AAIManager::AIGetManagerInstance(UWorld* WorldCtx) {
	return Cast<AAIManager>(UGameplayStatics::GetActorOfClass(WorldCtx, AAIManager::StaticClass()));
}

AAIManager::AAIManager() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAIManager::BeginPlay() {
	Super::BeginPlay();

	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AAINavNode::StaticClass(), FoundNodes);

	this->Nodes = TArray<AAINavNode*>();
	for (int i = 0; i < FoundNodes.Num(); i++) {
		this->Nodes.Push(Cast<AAINavNode>(FoundNodes[i]));
	}
}

void AAIManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AAIManager::AIIsNavNodeClaimed(AAINavNode* Node) {
	for (int i = 0; i < this->Claims.Num(); i++) {
		if (this->Claims[i].Node == Node) return true;
	}

	return false;
}

void AAIManager::AIClaimNavNode(AAINavNode* Node, AActor* Claimer) {
	FNavNodeClaim Claim;
	Claim.Claimer = Claimer;
	Claim.Node = Node;

	this->Claims.Push(Claim);
}

void AAIManager::AIUnclaimAllNavNodes(AActor* Claimer) {
	for (int i = 0; i < this->Claims.Num(); i++) {
		while (i < this->Claims.Num() && this->Claims[i].Claimer == Claimer) {
			this->Claims.RemoveAt(i);
		}
	}
}

AAINavNode* AAIManager::AIGetNavBestCoverNodeFor(AActor* For, AActor* From, int SearchCount, float MaxDistance) {
	FVector ForLocation = For->GetActorLocation();
    FVector ThreatLocation = From->GetActorLocation();

    TArray<AAINavNode*> CheckSet = this->AIGetNavNearestNodes(For, SearchCount);

    AAINavNode* FoundCover = nullptr;
    float CoverScore = 0.0f;
    for (int i = 0; i < CheckSet.Num(); i++) {
        AAINavNode* Check = CheckSet[i];

        if (!Check->CoverPosition) continue;
        if (this->AIIsNavNodeClaimed(Check)) continue;

        FVector CheckLocation = Check->GetActorLocation();
        if (MaxDistance > 0.0f && (CheckLocation - ForLocation).Size() > MaxDistance) continue;

        float RootDist = (CheckLocation - ThreatLocation).Size();
        float FrontDist = (CheckLocation + (Check->GetActorForwardVector() * 100.0f) - ThreatLocation).Size();
        if (FrontDist >= RootDist) continue;

        float CheckScore = FrontDist - RootDist;
        if (FoundCover == nullptr || CheckScore < CoverScore) {
            FoundCover = Check;
            CoverScore = CheckScore;
        }
    }

    return FoundCover;
}

TArray<AAINavNode*> AAIManager::AIGetNavNearestNodes(AActor* From, int Count) {
	// TODO: Rewrite, too lazy to to properly.

	FVector FromLocation = From->GetActorLocation();

	TArray<AAINavNode*> Result;
	while (Result.Num() < Count) {
		AAINavNode* Best = nullptr;
		float BestDistance = 0.0f;

		for (int i = 0; i < this->Nodes.Num(); i++) {
			AAINavNode* Check = this->Nodes[i];
			if (Result.Contains(Check)) continue;

			float CheckDistance = (Check->GetActorLocation() - FromLocation).Size();
			if (Best == nullptr || CheckDistance < BestDistance) {
				Best = Check;
				BestDistance = CheckDistance;
			}
		}

		if (Best == nullptr) break;
		Result.Push(Best);
	}

	return Result;
}

bool AAIManager::AIIsFactionEnemy(int MyFaction, int OtherFaction, bool GivenDamage) {
	if (MyFaction == OtherFaction) return false;

	for (int i = 0; i < this->FactionConfig.Num(); i++) {
		FFactionAlliance Check = this->FactionConfig[i];

		bool Allied = (
			Check.FactionA == MyFaction &&
			Check.FactionB == OtherFaction &&
			(!GivenDamage || Check.IgnoreDamage)	
		);
		if (Allied) return false;
	}

	return true;
}

bool AAIManager::AIShouldDetect(int FactionID, int Detection, AActor* RawTarget) {
	AUnitPawn* Target = Cast<AUnitPawn>(RawTarget);

	if (Target == nullptr) return false;
	if (!this->AIIsFactionEnemy(FactionID, Target->FactionID, false)) return false;
	if (Target->UnitGetConcealmentScore() > Detection) return false;

	return true;
}
