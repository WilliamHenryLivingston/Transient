#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

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
	UPROPERTY(EditAnywhere)
	float Speed = 200.0f;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 20.0f;

	UPROPERTY(EditAnywhere)
	float StrafeModifier = 0.5f;

	UPROPERTY(EditAnywhere)
	float StrafeConeAngle = 0.9f;

	UPROPERTY(EditAnywhere)
	float Health = 300.0f;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 300.0f;

	UPROPERTY(EditAnywhere)
	float Stamina = 300.0f;

	UPROPERTY(EditAnywhere);
	float MaxStamina = 300.0f;

	UPROPERTY(EditAnywhere)
	float InteractAnimTime = 1.0f;

	UPROPERTY(EditAnywhere)
	int MagazineCapacity;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;

	// Discovered children.
	UStaticMeshComponent* WeaponHostComponent;
	UStaticMeshComponent* ArmorHostComponent;
	UStaticMeshComponent* BackWeaponHostComponent;
	UStaticMeshComponent* MagazineReloadHostComponent;
	USkeletalMeshComponent* RigComponent;
	TArray<UStaticMeshComponent*> MagazineHostComponents;

	// Child class targeting.
	FVector MoveTarget;
	bool HasMoveTarget;
	FVector FaceTarget;
	bool HasFaceTarget;

	// Internal state.
	float ReloadTimer;
	float InteractTimer;

	bool HasStaminaDrain;

protected:
	UPROPERTY(EditAnywhere)
	FVector WeaponOffset;

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

	bool Crouching;

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

// Exposures.
private:
	void UnitTriggerGenericInteraction();

	void UnitUpdateMagazineMeshes();

	void UnitUpdateHostMesh(UStaticMeshComponent* Host, UStaticMesh* Target);

protected:
	void UnitPostTick(float DeltaTime);

	void UnitMoveTowards(FVector Target);

	void UnitFaceTowards(FVector Target);

	void UnitSetTriggerPulled(bool NewTriggerPulled);

	bool UnitDrainStamina(float Amount);

	bool UnitArmsOccupied();

	void UnitReload();

	void UnitSwapWeapons();

public:
	void UnitTakeDamage(FDamageProfile Profile);

	void UnitEquipItem(AItemActor* TargetItem);

	void UnitEquipWeapon(AWeaponItem* TargetWeapon);
	
	void UnitEquipArmor(AArmorItem* TargetArmor);

	void UnitEquipMagazine(AMagazineItem* TargetMagazine);

	void UnitDie();
};
