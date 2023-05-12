// Copyright: R. Saxifrage, 2023. All rights reserved.

// Units are fully dynamic agent characters.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/AudioComponent.h"

#include "Transient/Combat/Damagable.h"
#include "Transient/Items/ItemActor.h"
#include "Transient/Items/MagazineItem.h"
#include "Transient/Items/WeaponItem.h"
#include "Transient/Items/ArmorItem.h"
#include "Transient/Items/Inventory/InventoryComponent.h"
#include "Transient/Animation/UnitAnimInstance.h"
#include "Transient/Animation/LegIKSkeletonComponent.h"

#include "InteractiveAgent.h"

#include "UnitAgent.generated.h"

// TODO: >>>>CONST<<< UnitInventory()

// TODO: Shrink significantly, lots of biped specific stuff in here.

USTRUCT()
struct FLegIKTrackGroupConfig {
	GENERATED_BODY()

public:
	TArray<FVector> BaseComponentLocations;
};

USTRUCT() // TODO: Move.
struct FUnitConcealment {
	GENERATED_BODY()

public:
	AActor* Source;
	int Score;
	int ScoreCrouched;
};

// Because we can't use CRTP...
class UUnitDelegateAnimationCallback {
public:
	virtual void Callback();
};

UCLASS()
class AUnitAgent : public AAgentActor, public IDamagable {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float LegMovingOffsetTolerance = 40.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float LegRestingOffsetTolerance = 15.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float LegGroundCastDistance = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float LegStepVerticalOffset = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float LegStepBaseDistance = 150.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	TArray<FLegIKTrackGroupConfig> LegTrackGroups;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float BodyBaseOffset = 105.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit IK")
	float BodyLerpRate = 25.0f;

private:
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float MoveSpeed = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float TurnSpeed = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float LookPitchSpeed = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float LookPitchLimit = 30.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float StrafeModifier = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float JumpStrength = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float JumpTime = 1.0f; // TODO: Replace with proper grounded check.
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float SprintModifier = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Movement")
	float ExertedStaminaDrain = 100.0f;

	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float KineticHealth = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Stats")
	float MaxKineticHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Energy = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Stats")
	float MaxEnergy = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Stamina = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Stats");
	float MaxStamina = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Stats");
	float StaminaRegen = 50.0f;

	// Rig-related parameters.
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	FAnimationConfig InteractAnimation;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	float CrouchVerticalShrink = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	float CrouchVerticalTranslate = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	TSubclassOf<AActor> RagdollType;

	// Inventory.
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AItemActor* ActiveItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AArmorItem* ArmorItem;
	TArray<UUnitSlotComponent*> Slots;

	// Internal child components.
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
	AItemActor* CurrentUseItem;
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
	bool Slow;

	TArray<FUnitConcealment> ActiveConcealments;

protected:
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	TArray<TSubclassOf<AItemActor>> AutoSpawnInitialItems;

	bool ForceArmsEmptyAnimation; // TODO: Better solution (inventory view).
	bool CheckingStatus; // TODO

	UUnitDelegateAnimationCallback* DelegateAnimCallback;

	float AnimationScale;
	bool IgnoreTorsoYaw;

	// Child components available to child classes.
	ULegIKSkeletonComponent* RigComponent;
	UShapeComponent* ColliderComponent;
	UAudioComponent* AudioComponent;
	UUnitAnimInstance* Animation;
	USceneComponent* AimRootComponent;

public:
	// TODO: Private these later.
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float TakeReach = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float UseReach = 300.0f;

	bool OverrideArmsState; // Used to prevent validity checks and animation on arm-based actions.

public:
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	int FactionID = 1;

// AActor.
public:
	AUnitPawn();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

// IItemHolder.
public:
	virtual FVector ItemHolderGetLocation() override;
	virtual FRotator ItemHolderGetRotation() override;
	virtual void ItemHolderPlaySound(USoundBase* Sound) override;
	virtual float ItemHolderGetSpreadModifier() override;

// IDamagable.
public:
	virtual void DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) override;

// Internals.
private:
	void UnitRawSetActiveItem(AItemActor* Item);

protected:
	void ThenDelegateCallback();

private:
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
	void UnitDiscoverDynamicChildComponentsOf(TArray<UUnitSlotComponent*>& Into, AActor* Actor);

	// Must be called at the end of child-class ticks.
	void UnitPostTick(float DeltaTime);

public:
	// State exposures.
	bool UnitArmsOccupied();
	bool UnitIsJumping();
	bool UnitIsCrouched();
	bool UnitIsMoving();
	bool UnitIsExerted();
	float UnitGetEnergy();
	float UnitGetKineticHealth();
	int UnitGetConcealmentScore();
	AItemActor* UnitActiveItem();
	AWeaponItem* UnitActiveWeapon();
	AArmorItem* UnitGetArmor();

	// Inventory.
	void UnitDropActiveItem();
	void UnitDequipActiveItem();
	void UnitDropArmor();
	UUnitSlotComponent* UnitGetSlotWithItem(AItemActor* Target);
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
	void UnitSetTorsoPitch(float TargetValue);

	void UnitMoveTowards(FVector Target);
	void UnitFaceTowards(FVector Target);
	bool UnitHasFaceTarget();
	void UnitImmobilize(bool Which);
	void UnitSetSlow(bool Which);
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
	void UnitHealDamage(FDamageProfile Healing);
	void UnitTakeItem(AItemActor* TargetItem);
	virtual void UnitDie();

	void UnitAddConcealment(AActor* Source, int Score, int ScoreCrouched);
	void UnitRemoveConcealment(AActor* Source);
};
