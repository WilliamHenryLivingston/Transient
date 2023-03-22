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
	UPROPERTY(EditAnywhere)
	UStaticMesh* EquippedMesh;

	UPROPERTY(EditAnywhere)
	EUnitAnimArmsMode EquippedAnimArmsMode;

	UPROPERTY(EditAnywhere)
	float ReloadTime;

	UPROPERTY(EditAnywhere)
	bool CanHolster = true;

	UPROPERTY(EditAnywhere)
	int AmmoTypeID;

protected:
	UPROPERTY(EditAnywhere)
	FVector MuzzleLocation;

	bool TriggerPulled;

public:
	AWeaponItem();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void WeaponSetTriggerPulled(bool NewTriggerPulled);

	virtual void WeaponSwapMagazines(int NewAmmoCount);

	virtual bool WeaponEmpty();
};
