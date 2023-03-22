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

#include "UnitPawn.generated.h"

UCLASS()
class TRANSIENT_API AUnitPawn : public APawn, public IItemHolder {
	GENERATED_BODY()

private:
	// Movement.
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float Speed = 200.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float TurnSpeed = 20.0f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float StrafeModifier = 0.5f;
	UPROPERTY(EditAnywhere, Category="Unit Movement")
	float StrafeConeAngle = 0.9f;
	UPROPERTY(EditAnywhere, Category="Unit Movement");
	float JumpStrength = 300.0f;
	UPROPERTY(EditAnywhere)
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

	// Rig related parameters.
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

	// Per-tick pending updates from child classes.
	// Movement.
	FVector MoveTarget;
	FVector LastMoveTarget; // Tracks previous tick effective move target.
	bool HasMoveTarget;

	// XY-plane orientation.
	FVector FaceTarget;
	bool HasFaceTarget;

	// Arms state.
	float ReloadTimer;
	float InteractTimer;
	
	// Legs state.
	bool Crouching;

	float JumpTimer; // TODO: Remove once grounded check exists.
	FVector JumpMoveTarget; // No air control; used to continue last xy-plane movement.

	// Stat effects state.
	bool HasStaminaDrain;

protected:
	// Child components available to child classes.
	UPROPERTY(EditDefaultsOnly, Category="Unit Rig")
	UBoxComponent* ColliderComponent; // Exposed to allow configuration.
	UAudioComponent* AudioComponent;

	// Inventory state.
	UPROPERTY(EditAnywhere)
	AWeaponItem* WeaponItem;
	UPROPERTY(EditAnywhere)
	AWeaponItem* BackWeaponItem;
	UPROPERTY(EditAnywhere)
	AArmorItem* ArmorItem;
	UPROPERTY(EditAnywhere)
	TArray<AMagazineItem*> Magazines;

	UnitAnimInstance* Animation;

	bool OverrideArmState;
	
	int ActiveWeaponSlot;

	float RootPitch;

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

	virtual FVector ItemHolderGetWeaponOffset() override;

	virtual void ItemHolderPlaySound(USoundBase* Sound) override;

	virtual float ItemHolderGetSpreadModifier() override;

// Exposures.
private:
	void UnitTriggerGenericInteraction();

	void UnitUpdateMagazineMeshes();

	void UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config, bool AltRotation);

protected:
	void UnitPostTick(float DeltaTime);

	void UnitMoveTowards(FVector Target);

	void UnitFaceTowards(FVector Target);

	void UnitSetTriggerPulled(bool NewTriggerPulled);

	bool UnitDrainStamina(float Amount);

	bool UnitArmsOccupied();

	void UnitReload();

	void UnitJump();

	void UnitSetCrouched(bool NewCrouch);

	void UnitSwapWeapons();

public:
	void UnitTakeDamage(FDamageProfile Profile);

	void UnitEquipItem(AItemActor* TargetItem);

	void UnitEquipWeapon(AWeaponItem* TargetWeapon);
	
	void UnitEquipArmor(AArmorItem* TargetArmor);

	void UnitEquipMagazine(AMagazineItem* TargetMagazine);

	void UnitDie();
	bool UnitIsJumping();
	bool UnitIsCrouched();
};
