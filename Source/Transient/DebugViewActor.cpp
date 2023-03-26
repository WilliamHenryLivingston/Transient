#include "DebugViewActor.h"

#include "Components/StaticMeshComponent.h"

ADebugViewActor::ADebugViewActor() {
	PrimaryActorTick.bCanEverTick = true;
	
	this->DebugVisibility = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisibility"));
	this->DebugVisibility->SetupAttachment(this->RootComponent);
	this->DebugVisibility->bIsEditorOnly = true;
	this->DebugVisibility->SetRelativeScale3D(FVector(20.0f, 20.0f, 8.0f));
	this->DebugVisibility->SetVisibility(true);
}

void ADebugViewActor::BeginPlay() {
	Super::BeginPlay();

	this->DebugVisibility->SetVisibility(false);
}

void ADebugViewActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
