#pragma once

#include "CoreMinimal.h"

#include "WeaponItem.h"

#include "ProjectileWeapon.generated.h"

UCLASS()
class TRANSIENT_API AProjectileWeapon : public AWeaponItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Projectile Weapon")
	float Spread = 3.0f;

	UPROPERTY(EditAnywhere, Category="Item SFX")
	USoundBase* ShootSound;
	UPROPERTY(EditAnywhere, Category="Item SFX")
	USoundBase* EmptySound;

	UPROPERTY(EditAnywhere, Category="Projectile Weapon")
	float FireCooldownTime = 0.25f;

	// State.
	float FireCooldownTimer = 0.0f;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual bool WeaponEmpty() override;
};
