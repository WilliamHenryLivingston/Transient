#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

#include "UnitPawn.h"

#include "PlayerUnit.generated.h"

UCLASS()
class TRANSIENT_API APlayerUnit : public AUnitPawn
{
	GENERATED_BODY()

public:
	APlayerUnit();

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	FVector2D MovementInput;

protected:
	virtual void BeginPlay() override;

	virtual void OnUnitFace(FRotator Rotation) override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void InputStartFire();

	UFUNCTION()
	void InputStopFire();

	UFUNCTION()
	void InputForward(float AxisValue);

	UFUNCTION()
	void InputRight(float AxisValue);
};
