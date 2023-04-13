// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../DebugViewActor.h"
#include "../Items/ItemActor.h"

#include "AINavNode.generated.h"

UCLASS()
class TRANSIENT_API AAINavNode : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="AI Nav")
	TArray<AAINavNode*> Neighbors;

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
	float WaitTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category="Patrol")
	float WaitTimeMax = 1.0f;
	UPROPERTY(EditAnywhere, Category="Patrol")
	bool ScanWhileWaiting = true;

public:
	AAINavNode();
};
