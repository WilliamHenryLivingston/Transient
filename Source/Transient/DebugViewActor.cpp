// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "DebugViewActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

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

void ADebugViewActor::DebugSetLabel() {
	UTextRenderComponent* PrevLabel = this->FindComponentByClass<UTextRenderComponent>();
	if (PrevLabel != nullptr) PrevLabel->DestroyComponent();

	UTextRenderComponent* Label = NewObject<UTextRenderComponent>(this);
	this->AddInstanceComponent(Label);
	Label->RegisterComponent();

	Label->SetText(FText::FromString(this->DebugLabel));
	Label->AttachToComponent(this->DebugVisibility, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Label->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.2f));
	Label->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	Label->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));
	Label->bIsEditorOnly = true;
	Label->SetHiddenInGame(true);
}
