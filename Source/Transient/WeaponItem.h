// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectileActor.h"
#include "ItemActor.h"
#include "UnitAnimInstance.h"
#include "MagazineItem.h"

#include "WeaponItem.generated.h"

UCLASS()
class TRANSIENT_API AWeaponItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	FAnimationConfig ReloadAnimation;
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	int AmmoTypeID;
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	bool ImmobilizeOnReload;
	UPROPERTY(EditAnywhere, Category="Base Weapon")
	float ReloadMagazineAttachTime = 0.5f;

	UPROPERTY(EditAnywhere, Category="Item SFX")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, Category="AI Handling")
	float AIEngageDistance = 500.0f;

protected:
	bool TriggerPulled;

	AMagazineItem* ActiveMagazine;

	USceneComponent* MuzzlePosition;
	USceneComponent* ActiveMagazineHost;

public:
	AWeaponItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void WeaponSetTriggerPulled(bool NewTriggerPulled);
	bool WeaponGetTriggerPulled();
	void WeaponDisposeCurrentMagazine();
	FVector WeaponGetMuzzlePosition();
	FRotator WeaponGetMuzzleRotation();
	void WeaponSwapMagazines(AMagazineItem* NewMagazine);
	virtual bool WeaponEmpty();
};
