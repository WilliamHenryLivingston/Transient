// Copyright: R. Saxifrage, 2023. All rights reserved.

// Armor items absorb damage that would be dealt to its parent unit when equipped.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Combat/Damagable.h"

#include "ItemActor.h"

#include "ArmorItem.generated.h"

UCLASS()
class AArmorItem : public AItemActor, public IDamagable {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Armor")
	float MaxKineticHealth = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Armor")
	bool Heavy;

private:
	UPROPERTY(EditDefaultsOnly, Category="Armor")
	UStaticMesh* DamagedVariant;

	UPROPERTY(EditAnywhere, ReplicatedUsing=ArmorHealthChanged, Category="Armor")
	float KineticHealth = 300.0f;

	// Isomorphic, non-replicated.
	UStaticMesh* DefaultVariant;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	// Isomorphic.
	void ArmorHealthChanged();

public:
	// Game logic.
	virtual void DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
};
