#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

#include "UnitPawn.h"

#include "PlayerUnit.generated.h"

UCLASS()
class TRANSIENT_API APlayerUnit : public AUnitPawn {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	FVector2D MovementInput;

	float CurrentForcedDilation;

public:
	APlayerUnit();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnUnitFace(FRotator Rotation) override;

private:
	UFUNCTION()
	void InputStartFire();

	UFUNCTION()
	void InputStopFire();

	UFUNCTION()
	void InputInteract();

	UFUNCTION()
	void InputForward(float AxisValue);

	UFUNCTION()
	void InputRight(float AxisValue);

	UFUNCTION()
	void InputStartDilate();

	UFUNCTION()
	void InputStopDilate();
};
