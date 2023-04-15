// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "MetadataActor.h"

#if WITH_EDITOR
AMetadataActor::AMetadataActor() {
	this->EditorVisibilityComponent = this->CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("EditorVisibility"));
	this->EditorVisibilityComponent->SetupAttachment(this->RootComponent);
	this->EditorVisibilityComponent->bIsEditorOnly = true;
	this->EditorVisibilityComponent->SetRelativeScale3D(FVector(20.0f, 20.0f, 8.0f));
	this->EditorVisibilityComponent->SetVisibility(true);
	this->EditorVisibilityComponent->SetHiddenInGame(true);

	this->EditorLabelComponent = this->CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("EditorLabel"));
	this->EditorLabelComponent->SetupAttachment(this->EditorVisibility);
	this->EditorLabelComponent->bIsEditorOnly = true;
	this->EditorLabelComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.2f));
	this->EditorLabelComponent->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	this->EditorLabelComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));
	this->EditorLabelComponent->SetVisibility(true);
	this->EditorLabelComponent->SetHiddenInGame(true);
}

void AMetadataActor::SetEditorLabel() {
	this->EditorLabelComponent->SetText(FText::FromString(this->EditorLabel));
}
#endif
