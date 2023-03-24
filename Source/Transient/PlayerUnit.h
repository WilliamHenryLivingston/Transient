#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"

#include "UnitPawn.h"
#include "MainUIWidget.h"

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

	UPROPERTY(EditAnywhere, Category="Player Camera")
	float InventoryViewDistance = 350.0f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float InventoryViewCameraLerpRate = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float InventoryViewCameraPitchLerpRate = 170.0f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float InventoryViewCameraYawLerpRate = 180.0f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	TSubclassOf<UUserWidget> MainUIType;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float AimPadding = 0.1f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float AimSpeed = 500.0f;
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float AimMaxDistance = 500.0f;

	// Input.
	FVector2D MovementInput;

	// Other state.
	AActor* CurrentAimHit;

	UMainUIWidget* MainUI;

	float CurrentForcedDilation;
	bool WantsDilate;

	float StandardCameraArmZOffset;
	float StandardCameraArmLength;
	float StandardCameraPitch;
	FVector StandardAimIndicatorScale;

	bool InventoryView;
	float InventoryViewFaceTimer;
	bool DropInventoryFocused;

	bool Aiming;
	FVector AimCameraOffset;

// AActor.
public:
	APlayerUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
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
	void InputEquipSlotC();
	void InputEquipSlotD();
	void InputEquipSlotE();
	void InputStartCrouch();
	void InputEndCrouch();
	void InputJump();
	void InputDropActive();
	void InputEnterInventory();
	void InputExitInventory();
	void InputStartAim();
	void InputEndAim();
};
