// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"

#include "../Damagable.h"
#include "ItemActor.h"

#include "RepairItem.generated.h"

UCLASS()
class TRANSIENT_API ARepairItem : public AItemActor {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Repair Item")
	FDamageProfile Healing;

	UNiagaraComponent* UseFX;

protected:
	virtual void BeginPlay() override;

public:
	virtual void ItemStartUse() override;
	virtual void ItemUse(AActor* Target) override;
};
