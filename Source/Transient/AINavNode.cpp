#include "AINavNode.h"

AAINavNode::AAINavNode() {
 	PrimaryActorTick.bCanEverTick = false;
}

void AAINavNode::BeginPlay() {
	Super::BeginPlay();
}

void AAINavNode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
