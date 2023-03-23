#include "AINavNode.h"

#include "Components/SphereComponent.h"

AAINavNode::AAINavNode() {
 	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* Sphere = this->CreateDefaultSubobject<USphereComponent>(TEXT("DebugVisibility"));
	Sphere->SetupAttachment(this->RootComponent);
	Sphere->bIsEditorOnly = true;
	Sphere->SetVisibility(true);
}

void AAINavNode::BeginPlay() {
	Super::BeginPlay();
}

void AAINavNode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
