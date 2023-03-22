#include "AINavNode.h"

#include "Components/SphereComponent.h"

TArray<AAINavNode*> AAINavNode::WorldGraph = TArray<AAINavNode*>();
bool AAINavNode::WorldGraphComplete = false;

AAINavNode* AAINavNode::AINavGraphNearestNode(FVector Target) {
	float BestDistance;
	AAINavNode* Best = nullptr;

	for (int i = 0; i < AAINavNode::WorldGraph.Num(); i++) {
		AAINavNode* Check = AAINavNode::WorldGraph[i];

		float CheckDistance = (Target - Check->GetActorLocation()).Length();
		if (Best == nullptr || CheckDistance < BestDistance) {
			Best = Check;
			BestDistance = CheckDistance;
		}
	}

	return Best;
}

AAINavNode::AAINavNode() {
 	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* Sphere = this->CreateDefaultSubobject<USphereComponent>(TEXT("DebugVisibility"));
	Sphere->SetupAttachment(this->RootComponent);
	Sphere->bIsEditorOnly = true;
	Sphere->SetVisibility(true);
}

void AAINavNode::BeginPlay() {
	Super::BeginPlay();
	
	AAINavNode::WorldGraph.Push(this);
}

void AAINavNode::Tick(float DeltaTime) {
	if (!AAINavNode::WorldGraphComplete) {
		for (int i = 0; i < AAINavNode::WorldGraph.Num(); i++) {
			AAINavNode* Current = AAINavNode::WorldGraph[i];

			for (int j = 0; j < AAINavNode::WorldGraph.Num(); j++) {
				if (i == j) continue;

				AAINavNode* Check = AAINavNode::WorldGraph[j];

				if (Check->Neighbors.Contains(Current) && !Current->Neighbors.Contains(Check)) {
					Current->Neighbors.Push(Check);
				}
			}
		}

		AAINavNode::WorldGraphComplete = true;
	}

	Super::Tick(DeltaTime);
}
