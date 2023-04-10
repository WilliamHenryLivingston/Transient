// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "MagazineItem.h"

#include "BatteryItem.generated.h"

UCLASS()
class TRANSIENT_API ABatteryItem : public AMagazineItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Repair Item")
	FDamageProfile Healing;
	UPROPERTY(EditAnywhere, Category="Repair Item")
	TSubclassOf<AActor> FinishEffect;

public:
	virtual void ItemUse(AActor* Target) override;
};
