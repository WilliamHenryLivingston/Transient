#pragma once

#include "CoreMinimal.h"

#include "WeaponActor.h"

#include "ProjectileWeapon.generated.h"

UCLASS()
class TRANSIENT_API AProjectileWeapon : public AWeaponActor {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> ProjectileType;

	UPROPERTY(EditAnywhere)
	float FireCooldownTime = 0.25f;

	UPROPERTY(EditAnywhere)
	int MagazineCapacity = 10;

	UPROPERTY(EditAnywhere)
	float ReloadTime = 1.0f;

	float CurrentFireCooldown = 0.0f;

	int CurrentMagazine = 0;

public:
	virtual void Tick(float DeltaTime) override;
};
