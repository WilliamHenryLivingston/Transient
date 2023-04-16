// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Rep/ReplicatedEffectActor.h"

#include "MagazineItem.h"

#include "BatteryItem.generated.h"

UCLASS()
class ABatteryItem : public AMagazineItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Repair Item")
	FDamageProfile Healing; // TODO: Needs to be impacted by remaining ammo.
	UPROPERTY(EditAnywhere, Category="Repair Item")
	TSubclassOf<AReplicatedEffectActor> FinishEffect;

public:
	// Game logic.
	virtual void ItemUse_Implementation(AActor* Target) override;
};
