// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/AudioComponent.h"

#include "Items/ItemActor.h"
#include "Items/UsableItem.h"
#include "Items/MagazineItem.h"
#include "Items/WeaponItem.h"
#include "Items/ArmorItem.h"
#include "Environment/InteractiveActor.h"
#include "Animation/UnitAnimInstance.h"
#include "UnitSlotComponent.h"

#include "UnitPawn.generated.h"

UCLASS()
class TRANSIENT_API AUnitPawn : public APawn, public IItemHolder {
	GENERATED_BODY()

// Variables.
private:
	// Movement.
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float MoveSpeed = 200.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float TurnSpeed = 20.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float LookPitchSpeed = 20.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float LookPitchLimit = 30.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float StrafeModifier = 0.5f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float StrafeConeAngle = 0.9f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float JumpStrength = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float JumpTime = 1.0f; // TODO: Replace with proper grounded check.
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float SprintModifier = 2.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float ExertedStaminaDrain = 100.0f;

	// Stats.
protected: // TODO: No, add methods.
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float KineticHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float MaxKineticHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Energy = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float MaxEnergy = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Stamina = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats");
	float MaxStamina = 300.0f;
private:
	UPROPERTY(EditAnywhere, Category="Unit Stats");
	float StaminaRegen = 50.0f;

	// Rig-related parameters.
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	FAnimationConfig InteractAnimation;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	TArray<FAnimationConfig> MiscArmsAnimations;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	float CrouchVerticalShrink = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	float CrouchVerticalTranslate = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	TSubclassOf<AActor> RagdollType;

	// Inventory.
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	TArray<TSubclassOf<AItemActor>> AutoSpawnInitialItems;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AItemActor* ActiveItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AArmorItem* ArmorItem;
	TArray<UUnitSlotComponent*> Slots;

	// Internal child components.
	USkeletalMeshComponent* RigComponent;
	USceneComponent* ActiveItemHostComponent;
	USceneComponent* ActiveItemAltHostComponent;

	// Per-tick pending updates from child classes.
	// Movement.
	FVector MoveTarget;
	bool HasMoveTarget;

	// XY-plane orientation.
	FVector FaceTarget;
	bool HasFaceTarget;
	
	// Deferred action states.
	AUsableItem* CurrentUseItem;
	AInteractiveActor* CurrentInteractActor;
	AMagazineItem* LoadingMagazine;
	AMagazineItem* UnloadingMagazine;
	AActor* CurrentUseItemTarget;

	// Rig state.
	// TODO: Better anim timing system.
	float JumpTimer; // TODO: Remove once grounded check exists.

	EUnitAnimArmsModifier ArmsAnimation;
	float ArmsAnimationTimer;
	float ArmsAnimationCooldownTimer;
	void (AUnitPawn::*ArmsAnimationThen)();
	
	bool Crouching;
	bool Exerted; // Sprint, time dialation, etc.

	float TorsoPitch;
	float TargetTorsoPitch;

	float BaseColliderVerticalScale;
	float BaseRigVerticalOffset;
	FVector BaseRigScale;

	// Other state.
	float StaminaRegenTimer;

	bool ArmsActionMoveLock;

	bool Immobilized;

protected:
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float TakeReach = 300.0f; // TODO: Private later.
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float UseReach = 300.0f;

	bool ForceArmsEmptyAnimation; // TODO: Better solution (inventory view).
	bool CheckingStatus; // TODO

	float AnimationScale;
	bool IgnoreTorsoYaw;

	// Child components available to child classes.
	UShapeComponent* ColliderComponent;
	UAudioComponent* AudioComponent;
	UUnitAnimInstance* Animation;
	USceneComponent* AimRootComponent;

public:
	bool OverrideArmsState; // Used to prevent validity checks and animation on arm-based actions.

public:
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	int FactionID = 1;

// AActor methods.
public:
	AUnitPawn();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

// IItemHolder methods.
public:
	virtual FVector ItemHolderGetLocation() override;
	virtual FRotator ItemHolderGetRotation() override;
	virtual void ItemHolderPlaySound(USoundBase* Sound) override;
	virtual float ItemHolderGetSpreadModifier() override;

// Internals.
private:
	void UnitDequipActiveItem();
	void UnitRawSetActiveItem(AItemActor* Item);

	void ThenFinishUse();
	void ThenPostReload();
	// Reload animation phases:
	void ThenStartReload();
	void ThenEarlyReload();
	void ThenMidReload();
	void ThenFinishInteract();

// Exposures.
protected:
	virtual void UnitDiscoverDynamicChildComponents();

	// Must be called at the end of child-class ticks.
	void UnitPostTick(float DeltaTime);

public:
	// State exposures.
	bool UnitAreArmsOccupied();
	bool UnitIsJumping();
	bool UnitIsCrouched();
	bool UnitIsExerted();
	AItemActor* UnitGetActiveItem();
	AWeaponItem* UnitGetActiveWeapon();
	AArmorItem* UnitGetArmor();

	// Inventory.
	void UnitDropActiveItem();
	void UnitDropArmor();
	bool UnitHasItem(AItemActor* Target);
	AItemActor* UnitGetItemByName(FString ItemName);
	AItemActor* UnitGetItemByClass(TSubclassOf<AItemActor> ItemClass);
	void UnitDropItem(AItemActor* Target);
	void UnitEquipItem(AItemActor* Target);
	void UnitEquipFromSlot(int Index);
	TArray<UUnitSlotComponent*> UnitGetEquippableSlots();
	UUnitSlotComponent* UnitGetEmptySlotAllowing(EItemInventoryType Type);
	UUnitSlotComponent* UnitGetSlotAllowing(EItemInventoryType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContaining(EItemInventoryType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContainingMagazines(int AmmoTypeID);
	
	// Actions.
	bool UnitDrainStamina(float Amount);
	bool UnitDrainEnergy(float Amount);
	void UnitUpdateTorsoPitch(float TargetValue);

	void UnitMoveTowards(FVector Target);
	void UnitFaceTowards(FVector Target);
	bool UnitHasFaceTarget();
	void UnitImmobilize(bool Which);
	void UnitJump();
	void UnitSetCrouched(bool NewCrouch);
	void UnitSetExerted(bool NewSprint);

	void UnitUseActiveItem(AActor* Target);
	void UnitInteractWith(AActor* Target);
	void UnitSetTriggerPulled(bool NewTriggerPulled);
	virtual void UnitReload();

	void UnitPlayAnimationOnce(EUnitAnimArmsModifier Animation, FAnimationConfig Config, void (AUnitPawn::*Then)());
	void UnitSetCheckingStatus(bool NewChecking);
	void UnitPlayInteractAnimation();

	// External impacts.
	virtual void UnitTakeDamage(FDamageProfile Profile, AActor* Source);
	void UnitHealDamage(FDamageProfile Healing);
	void UnitTakeItem(AItemActor* TargetItem);
	void UnitDie();
};
