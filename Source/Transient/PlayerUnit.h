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
	// Child components.
	UPROPERTY(EditAnywhere, Category="Player Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	USpringArmComponent* CameraArmComponent;
	UStaticMeshComponent* AimIndicatorComponent;

	// Input.
	FVector2D MovementInput;

	// Other state.
	AActor* CurrentAimHit;

	float CurrentForcedDilation;
	bool WantsDilate;

// AActor.
public:
	APlayerUnit();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
// AUnitPawn overrides.
	virtual void UnitDiscoverChildComponents() override;

// Input binds.
	void InputStartFire();

	void InputStopFire();

	void InputInteract();

	void InputForward(float AxisValue);

	void InputRight(float AxisValue);

	void InputStartDilate();

	void InputStopDilate();

	void InputReload();
	
	void InputEquipSlotA();
	
	void InputEquipSlotB();

	void InputStartCrouch();

	void InputEndCrouch();

	void InputJump();
};
