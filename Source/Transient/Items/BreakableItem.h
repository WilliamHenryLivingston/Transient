// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../Damagable.h"
#include "ItemActor.h"

#include "BreakableItem.generated.h"

UCLASS()
class TRANSIENT_API ABreakableItem : public AItemActor, public IDamagable{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	float KineticHealth = 400.0f;
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	float BreakImpulseStrength = 50.0f;
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	TArray<TSubclassOf<AActor>> SpawnOnBreak;

public:
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
};
