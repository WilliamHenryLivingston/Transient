#include "AINavManager.h"

#include "Kismet/GameplayStatics.h"

AAINavManager::AAINavManager() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAINavManager::BeginPlay() {
	Super::BeginPlay();

	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AAINavNode::StaticClass(), FoundNodes);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(FoundNodes.Num()));

	this->Nodes = TArray<AAINavNode*>();
	for (int i = 0; i < FoundNodes.Num(); i++) {
		this->Nodes.Push(Cast<AAINavNode>(FoundNodes[i]));
	}
}

void AAINavManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

TArray<AAINavNode*> AAINavManager::NavGetNearestNodes(AActor* From, int Count) {
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