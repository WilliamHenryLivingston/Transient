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

	// Inventory state; exposed to allow initial state setup.
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AWeaponItem* WeaponItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AWeaponItem* BackWeaponItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	AArmorItem* ArmorItem;
	UPROPERTY(EditAnywhere, Category="Unit Inventory")
	TArray<AMagazineItem*> Magazines;

	// Rig-related parameters.
	UPROPERTY(EditAnywhere, Category="Unit Rig")
	float InteractAnimationTime = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	FVector WeaponOffset;

	// Internal child components.
	USkeletalMeshComponent* RigComponent;
	UStaticMeshComponent* WeaponHostComponent;
	UStaticMeshComponent* ArmorHostComponent;
	UStaticMeshComponent* BackWeaponHostComponent;
	UStaticMeshComponent* MagazineReloadHostComponent;
	TArray<UStaticMeshComponent*> MagazineHostComponents;

	TArray<UUnitSlotComponent*> Slots;

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
	float ReloadTimer;
	float InteractTimer;
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
	void UnitPlayGenericInteractionAnimation();
	void UnitUpdateMagazineHosts();
	void UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config, bool AltRotation);

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
	void UnitSwapWeapons(); // x
	void UnitSetCrouched(bool NewCrouch);
	void UnitUpdateTorsoPitch(float TargetValue);

public:
	// State exposures.
	bool UnitAreArmsOccupied();
	bool UnitIsJumping();
	bool UnitIsCrouched();
	AWeaponItem* UnitGetActiveWeapon();

	int UnitGetMagazineCountForAmmoType(int TypeID); // x

	void UnitEquipFromSlot(int Index);
	TArray<UUnitSlotComponent*> UnitGetSlotsAllowing(EItemEquipType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContaining(EItemEquipType Type);
	TArray<UUnitSlotComponent*> UnitGetSlotsContainingMagazines(int AmmoTypeID);

	// Public actions.
	void UnitTakeDamage(FDamageProfile Profile);
	void UnitEquipItem(AItemActor* TargetItem);
	void UnitEquipWeapon(AWeaponItem* TargetWeapon); // x
	void UnitEquipArmor(AArmorItem* TargetArmor); // x
	void UnitEquipMagazine(AMagazineItem* TargetMagazine); // x
	void UnitDie();
};
