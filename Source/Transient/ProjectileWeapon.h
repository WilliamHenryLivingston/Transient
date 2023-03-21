#pragma once

#include "CoreMinimal.h"

#include "WeaponItem.h"

#include "ProjectileWeapon.generated.h"

UCLASS()
class TRANSIENT_API AProjectileWeapon : public AWeaponItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> ProjectileType;

	UPROPERTY(EditAnywhere)
	float FireCooldownTime = 0.25f;

	UPROPERTY(EditAnywhere)
	int MagazineCapacity = 10;

	float CurrentFireCooldown = 0.0f;

	int CurrentMagazine = 0;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void WeaponSwapMagazines(int NewAmmoCount) override;

	virtual bool WeaponEmpty() override;
};
