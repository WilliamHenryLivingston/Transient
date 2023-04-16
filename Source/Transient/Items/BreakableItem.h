// Copyright: R. Saxifrage, 2023. All rights reserved.

// An item which can be broken by damage when in the world.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Combat/Damagable.h"

#include "ItemActor.h"
#include "LootPool.h"

#include "BreakableItem.generated.h"

UCLASS()
class ABreakableItem : public AItemActor, public IDamagable {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	float KineticHealth = 400.0f;
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	float BreakImpulseStrength = 50.0f;
	UPROPERTY(EditAnywhere, Category="Breakable Item")
	TSubclassOf<ULootPool> ContentLootPool;

	UPROPERTY(ReplicatedUsing=BreakableBrokenChanged)
	bool Broken;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Isomorphic.
	virtual void BreakableBrokenChanged();

public:
	// Game logic.
	virtual void DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
};
