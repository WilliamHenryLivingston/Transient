#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

#include "ItemActor.h"
#include "MagazineItem.h"
#include "WeaponItem.h"
#include "ArmorItem.h"
#include "UnitAnimInstance.h"
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
	float UseReach = 100.0f;

	// Stats.
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Health = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float MaxHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats")
	float Stamina = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats");
	float MaxStamina = 300.0f;
	UPROPERTY(EditAnywhere, Category="Unit Stats");
	float StaminaRegen = 50.0f;

	// Rig-related parameters.
	UPROPERTY(EditAnywhere, Category="Unit Rig")
	float InteractAnimationTime = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	FVector WeaponOffset;

	// Inventory.
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AItemActor* ActiveItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AArmorItem* ArmorItem;
	TArray<UUnitSlotComponent*> Slots;

	// Internal child components.
	USkeletalMeshComponent* RigComponent;
	UStaticMeshComponent* ActiveItemHostComponent;
	UStaticMeshComponent* ArmorHostComponent;

	// Per-tick pending updates from child classes.
	// Movement.
	FVector MoveTarget;
	FVector LastMoveTarget; // Tracks previous tick effective move target.
	bool HasMoveTarget;
	bool LastHasMoveTarget;

	// XY-plane orientation.
	FVector FaceTarget;
	bool HasFaceTarget;

	// Rig state.
	// TODO: Better anim timing system.
	float ReloadTimer;
	float InteractTimer;
	float UseTimer;
	float JumpTimer; // TODO: Remove once grounded check exists.
	
	bool Crouching;

	float TorsoPitch;
	float TargetTorsoPitch;

	// Other state.
	bool HasStaminaDrain;

protected:
	// Child components available to child classes.
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	UBoxComponent* ColliderComponent; // Exposed to allow configuration.
	UAudioComponent* AudioComponent;
	UUnitAnimInstance* Animation;

	bool OverrideArmState; // Used to prevent validity checks and animation on arm-based actions.

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
	virtual FVector ItemHolderGetWeaponOffset() override;
	virtual void ItemHolderPlaySound(USoundBase* Sound) override;
	virtual float ItemHolderGetSpreadModifier() override;

// Internals.
private:
	void UnitDequipActiveItem();
	void UnitPlayGenericInteractionAnimation();
	void UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config);

// Exposures.
protected:
	virtual void UnitDiscoverChildComponents();

	// Must be called at the end of child-class ticks.
	void UnitPostTick(float DeltaTime);

	// Internal-only actions.
	void UnitMoveTowards(FVector Target);
	void UnitFaceTowards(FVector Target);
	bool UnitDrainStamina(float Amount);
	void UnitSetTriggerPulled(bool NewTriggerPulled);
	void UnitReload();
	void UnitJump();
	void UnitUseActiveItem(AActor* Target);
	void UnitSetCrouched(bool NewCrouch);
	void UnitUpdateTorsoPitch(float TargetValue);

public:
	// State exposures.
	bool UnitAreArmsOccupied();
	bool UnitIsJumping();
	bool UnitIsCrouched();
	AItemActor* UnitGetActiveItem();
	AWeaponItem* UnitGetActiveWeapon();

	// Inventory.
	void UnitDropActiveItem();
	void UnitEquipFromSlot(int Index);
	TArray<UUnitSlotComponent*> UnitGetEquippableSlots();
	TArray<UUnitSlotComponent*> UnitGetEmptySlotsAllowing(EItemInventoryType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsAllowing(EItemInventoryType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContaining(EItemInventoryType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContainingMagazines(int AmmoTypeID);

	// Other actions.
	void UnitTakeDamage(FDamageProfile Profile);
	void UnitTakeItem(AItemActor* TargetItem);
	void UnitDie();
};
