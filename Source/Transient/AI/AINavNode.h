// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../DebugViewActor.h"

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

	UPROPERTY(EditAnywhere, Category="Patrol Travel")
    float NodeReachDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category="Patrol Equip")
	bool FindEquipItemInWorld;
	UPROPERTY(EditAnywhere, Category="Patrol Equip")
	TSubclassOf<AItemActor> EquipItemType;

	UPROPERTY(EditAnywhere, Category="Patrol Animation")
	int UseItemTimes = -1;
    UPROPERTY(EditAnywhere, Category="Patrol Animation")
    AActor* UseItemTarget;

	UPROPERTY(EditAnywhere, Category="Patrol Wait")
	float WaitTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category="Patrol Wait")
	float WaitTimeMax = 1.0f;
	UPROPERTY(EditAnywhere, Category="Patrol Wait")
	bool ScanWhileWaiting = true;

public:
	AAINavNode();
};
