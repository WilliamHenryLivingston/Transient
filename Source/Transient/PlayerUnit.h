#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "UnitPawn.h"

#include "PlayerUnit.generated.h"

UCLASS()
class TRANSIENT_API APlayerUnit : public AUnitPawn {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraArmComponent;

	FVector2D MovementInput;

	bool WantsDilate;

	float CurrentForcedDilation;

	UStaticMeshComponent* AimIndicatorComponent;

public:
	APlayerUnit();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	void InputStartFire();

	void InputStopFire();

	void InputInteract();

	void InputForward(float AxisValue);

	void InputRight(float AxisValue);

	void InputStartDilate();

	void InputStopDilate();

	void InputReload();
	
	void InputSwapWeaponA();
	
	void InputSwapWeaponB();

	void InputStartCrouch();

	void InputEndCrouch();
};
