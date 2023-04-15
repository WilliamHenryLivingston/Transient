// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentManager.h"

#include "Kismet/GameplayStatics.h"

#include "Units/UnitAgent.h"

AAgentManager* AAgentManager::AgentsGetManager(UWorld* WorldCtx) {
	return Cast<AAIManager>(UGameplayStatics::GetActorOfClass(WorldCtx, AAgentManager::StaticClass()));
}

AAgentManager::AAgentManager() {
	this->SetReplicates(true);
}

void AAgentManager::BeginPlay() {
	Super::BeginPlay();

	if (!this->HasAuthority()) return;

	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AAINavNode::StaticClass(), FoundNodes);

	this->Nodes = TArray<AAINavNode*>();
	for (int i = 0; i < FoundNodes.Num(); i++) {
		this->Nodes.Push(Cast<AAINavNode>(FoundNodes[i]));
	}
}

void AAgentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAgentManager, this->FactionConfig);
}

bool AAgentManager::AgentsFactionsAreEnemy(AAgentActor* A, AAgentActor* B, bool GivenDamage) {
	int AFaction = A->AgentFactionID();
	int BFaction = B->AgentFactionID();
	if (AFaction == BFaction) return false;

	for (int i = 0; i < this->FactionConfig.Num(); i++) {
		FFactionAlliance Check = this->FactionConfig[i];

		bool Allied = (
			Check.FactionA == AFaction &&
			Check.FactionB == BFaction &&
			(!GivenDamage || Check.IgnoreDamage)	
		);
		if (Allied) return false;
	}

	return true;
}

bool AAgentManager::UnitAIIsNavNodeClaimed(AAINavNode* Node) const {
	for (int i = 0; i < this->Claims.Num(); i++) {
		if (this->Claims[i].Node == Node) return true;
	}

	return false;
}

void AAgentManager::UnitAIClaimNavNode(AAINavNode* Node, AAIUnit* Claimer) {
	FNavNodeClaim Claim;
	Claim.Claimer = Claimer;
	Claim.Node = Node;

	this->Claims.Push(Claim);
}

void AAgentManager::UnitAIUnclaimAllNavNodesFor(AAIUnit* Claimer) {
	for (int i = 0; i < this->Claims.Num(); i++) {
		while (i < this->Claims.Num() && this->Claims[i].Claimer == Claimer) {
			this->Claims.RemoveAt(i);
		}
	}
}

AAINavNode* AAgentManager::UnitAIGetBestCoverNavNodeFor(AAIUnit* For, AActor* From, int SearchCount, float MaxDistance) {
	// TODO: Rewrite from spatial partition (octree?).
	FVector ForLocation = For->GetActorLocation();
    FVector ThreatLocation = From->GetActorLocation();

    TArray<AAINavNode*> CheckSet = this->UnitAIGetNearestNavNodesFor(For, SearchCount);

    AAINavNode* FoundCover = nullptr;
    float CoverScore = 0.0f;
    for (int i = 0; i < CheckSet.Num(); i++) {
        AAINavNode* Check = CheckSet[i];

        if (!Check->CoverPosition) continue;
        if (this->AgentAIIsNavNodeClaimed(Check)) continue;

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

TArray<AAINavNode*> AAgentManager::UnitAIGetNearestNavNodesFor(AAIUnit* For, int Count) {
	// TODO: Rewrite, too lazy to to properly.
	FVector ForLocation = For->GetActorLocation();

	TArray<AAINavNode*> Result;
	while (Result.Num() < Count) {
		AAINavNode* Best = nullptr;
		float BestDistance = 0.0f;

		for (int i = 0; i < this->Nodes.Num(); i++) {
			AAINavNode* Check = this->Nodes[i];
			if (Result.Contains(Check)) continue;

			float CheckDistance = (Check->GetActorLocation() - ForLocation).Size();
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
