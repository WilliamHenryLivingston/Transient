// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"

#endif

#include "MetadataActor.generated.h"

UCLASS()
class AMetadataActor : public AActor {
	GENERATED_BODY()

#if WITH_EDITOR
public:
	UPROPERTY(EditAnywhere, Category="Editor Label")
	FString EditorLabel;

private:
	UStaticMeshComponent* EditorVisibilityComponent;
	UTextRenderComponent* EditorLabelComponent;

public:
	AMetadataActor();

public:
	UFUNCTION(CallInEditor, Category="Editor Label")
	void SetEditorLabel();
#endif
};
