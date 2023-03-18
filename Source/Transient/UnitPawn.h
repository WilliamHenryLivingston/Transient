#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "WeaponActor.h"
#include "UnitAnimInstance.h"

#include "UnitPawn.generated.h"

UCLASS()
class TRANSIENT_API AUnitPawn : public APawn {
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
	UBoxComponent* ColliderComponent;

	UPROPERTY(EditInstanceOnly)
	AWeaponActor* Weapon;

	USkeletalMeshComponent* RigComponent;
	UnitAnimInstance* Animation;

	FVector MoveTarget;
	bool HasMoveTarget;

	FVector FaceTarget;
	bool HasFaceTarget;

public:
	AUnitPawn();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

protected:
	void UnitPostTick(float DeltaTime);

	void UnitMoveTowards(FVector Target);

	void UnitFaceTowards(FVector Target);

	void UnitSetTriggerPulled(bool NewTriggerPulled);

	TArray<AWeaponActor*> UnitGetNearbyWeapons();
	
public:
	void UnitTakeDamage(FDamageProfile* Profile);

	void UnitEquipWeapon(AWeaponActor* TargetWeapon);

	AWeaponActor* UnitGetWeapon();
	
	void UnitDie();
};
