#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectileActor.h"
#include "ItemActor.h"
#include "UnitAnimInstance.h"

#include "WeaponItem.generated.h"

UCLASS()
class TRANSIENT_API AWeaponItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	FAnimationConfig ReloadAnimation;
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	int AmmoTypeID;

	UPROPERTY(EditAnywhere, Category="AI Handling")
	float AIEngageDistance = 500.0f;

protected:
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	FVector MuzzleLocation;

	bool TriggerPulled;

public:
	AWeaponItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void WeaponSetTriggerPulled(bool NewTriggerPulled);
	bool WeaponGetTriggerPulled();
	FVector WeaponGetRelativeMuzzleAsEquipped();
	virtual void WeaponSwapMagazines(int NewAmmoCount);
	virtual bool WeaponEmpty();
};
