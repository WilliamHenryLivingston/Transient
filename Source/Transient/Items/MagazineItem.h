// Copyright: R. Saxifrage, 2023. All rights reserved.

// Magazines contain ammunition of a given type.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Combat/ProjectileActor.h"

#include "ItemActor.h"

#include "MagazineItem.generated.h"

// TODO(LOW): Separate bases for projectile/energy.

UCLASS()
class AMagazineItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Magazine")
	TSubclassOf<AProjectileActor> ProjectileType;
	UPROPERTY(EditDefaultsOnly, Category="Magazine")
	int Capacity;
	UPROPERTY(EditDefaultsOnly, Category="Magazine")
	int AmmoTypeID;

private:
	UPROPERTY(EditDefaultsOnly, Category="Magazine")
	TArray<UStaticMesh*> StateVariants;

	UPROPERTY(EditAnywhere, ReplicatedUsing=MagazineAmmoChanged, Category="Magazine")
	int Ammo;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Isomorphic.
	int MagazineAmmo();

	// Game logic.
	bool MagazineDeplete(int Amount);

private:
	// Isomorphic.
	void MagazineAmmoChanged();
};
