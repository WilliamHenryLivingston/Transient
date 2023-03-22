#pragma once

#include "CoreMinimal.h"

#include "WeaponItem.h"

#include "ProjectileWeapon.generated.h"

UCLASS()
class TRANSIENT_API AProjectileWeapon : public AWeaponItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float Spread = 3.0f; // Would be cool set in ammo.

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> ProjectileType;

	UPROPERTY(EditAnywhere)
	float FireCooldownTime = 0.25f;

	float CurrentFireCooldown = 0.0f;

	int CurrentMagazine = 0;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void WeaponSwapMagazines(int NewAmmoCount) override;

	virtual bool WeaponEmpty() override;
};
