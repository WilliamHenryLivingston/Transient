#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "ItemActor.h"
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
	float InteractAnimTime = 1.0f;

	UPROPERTY(EditAnywhere)
	FVector WeaponOffset;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;

	// Discovered children.
	UStaticMeshComponent* WeaponHostComponent;
	UStaticMeshComponent* ArmorHostComponent;
	UStaticMeshComponent* BackWeaponHostComponent;
	USkeletalMeshComponent* RigComponent;
	UnitAnimInstance* Animation;

	// Child class targeting.
	FVector MoveTarget;
	bool HasMoveTarget;
	FVector FaceTarget;
	bool HasFaceTarget;

	// Timers.
	float ReloadTimer;
	float InteractTimer;

protected:
	UPROPERTY(EditInstanceOnly)
	AWeaponItem* WeaponItem;
	
	UPROPERTY(EditInstanceOnly)
	AWeaponItem* BackWeaponItem;
	
	UPROPERTY(EditInstanceOnly)
	AArmorItem* ArmorItem;

private:
	bool OverrideArmState;

public:
	AUnitPawn();

	virtual void Tick(float DeltaTime) override;

	virtual FVector ItemHolderGetLocation() override;

	virtual FRotator ItemHolderGetRotation() override;

	virtual FVector ItemHolderGetWeaponOffset() override;

protected:
	virtual void BeginPlay() override;

private:
	void UnitTriggerGenericInteraction();

protected:
	void UnitPostTick(float DeltaTime);

	void UnitMoveTowards(FVector Target);

	void UnitFaceTowards(FVector Target);

	void UnitSetTriggerPulled(bool NewTriggerPulled);

	bool UnitArmsOccupied();

	void UnitReload();
	
public:
	void UnitTakeDamage(FDamageProfile Profile);

	void UnitEquipWeapon(AWeaponItem* TargetWeapon);
	
	void UnitEquipArmor(AArmorItem* TargetArmor);

	void UnitDie();
};
