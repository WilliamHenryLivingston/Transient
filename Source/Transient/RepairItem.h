#pragma once

#include "CoreMinimal.h"

#include "UsableItem.h"

#include "RepairItem.generated.h"

UCLASS()
class TRANSIENT_API ARepairItem : public AUsableItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Repair Item")
	FDamageProfile Healing;

public:
	virtual void ItemUse(AActor* Target) override;
};
