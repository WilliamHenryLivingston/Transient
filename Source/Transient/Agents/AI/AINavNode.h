// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Transient/MetadataActor.h"
#include "Transient/Items/ItemActor.h"
#include "Transient/Agents/InteractiveAgent.h"

#include "AINavNode.generated.h"

UCLASS()
class AAINavNode : public AMetadataActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="AI Nav")
	bool CoverPosition;
	UPROPERTY(EditAnywhere, Category="AI Nav")
	bool CrouchFullCover;

	UPROPERTY(EditAnywhere, Category="Patrol")
    float ReachDistance = 50.0f;
	UPROPERTY(EditAnywhere, Category="Patrol")
    AAINavNode* ForceNext;

	UPROPERTY(EditAnywhere, Category="Patrol")
	bool EquipItem;
	UPROPERTY(EditAnywhere, Category="Patrol")
	bool TryFindEquipItemInWorld;
	UPROPERTY(EditAnywhere, Category="Patrol")
	float FindEquipItemMaxDistance = 500.0f;
	UPROPERTY(EditAnywhere, Category="Patrol")
	TSubclassOf<AItemActor> EquipItemType;

	UPROPERTY(EditAnywhere, Category="Patrol")
	AActor* SubjugateTarget;

	UPROPERTY(EditAnywhere, Category="Patrol")
	bool UseItem;
    UPROPERTY(EditAnywhere, Category="Patrol")
    AActor* UseItemTarget;

	UPROPERTY(EditAnywhere, Category="Patrol")
	AInteractiveAgent* InteractTarget;

	UPROPERTY(EditAnywhere, Category="Patrol")
	float WaitTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category="Patrol")
	float WaitTimeMax = 1.0f;
	UPROPERTY(EditAnywhere, Category="Patrol")
	bool ScanWhileWaiting = true;
};
