// Copyright: R. Saxifrage, 2023. All rights reserved.

// Weapon items take magazines and are reloaded. Firing is handled by derived
// classes.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Animation/UnitAnimInstance.h"
#include "Transient/Rep/ReplicatedSoundComponent.h"

#include "Inventory/InventorySlotComponent.h"
#include "Inventory/InventoryComponent.h"
#include "ItemActor.h"
#include "MagazineItem.h"

#include "WeaponItem.generated.h"

UCLASS()
class AWeaponItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FAnimationConfig ReloadAnimation;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float ReloadMagazineAttachTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	int AmmoTypeID;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	bool ImmobilizeOnReload;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FRotator LaserCorrection;

	UPROPERTY(EditDefaultsOnly, Category="AI Handling")
	float AIFireDistance = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category="AI Handling")
	TSubclassOf<AMagazineItem> AutoSpawnMagazine;

protected:
	// Isomorphic, non-replicated.
	UInventoryComponent* Inventory;
	UInventorySlotComponent* MagazineSlot;

	UReplicatedSoundComponent* ReloadSound;
	UReplicatedSoundComponent* EmptySound;

private:
	USceneComponent* MuzzleLocation;

	// Isomorphic.
	UPROPERTY(Replicated)
	bool TriggerPulled;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// Isomorphic.
	bool WeaponTriggerPulled();
	bool WeaponEmpty();
	FVector WeaponMuzzleLocation();
	AMagazineItem* WeaponMagazine();

	// Game logic.
	void WeaponSetTriggerPulled(bool NewTriggerPulled);
	AMagazineItem* WeaponSwapMagazines(AMagazineItem* NewMagazine);
};
