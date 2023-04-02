// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "UnitPawn.h"
#include "MainUIWidget.h"
#include "StatusUIWidget.h"

#include "PlayerUnit.generated.h"

UCLASS()
class TRANSIENT_API APlayerUnit : public AUnitPawn {
	GENERATED_BODY()

private:
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
	UPROPERTY(EditAnywhere, Category="Player Camera")
	float SlowEffectStrength = 50.0f;

	// Child components.
	UCameraComponent* CameraComponent;
	USpringArmComponent* CameraArmComponent;
	UStaticMeshComponent* AimIndicatorComponent;

	// State.
	FVector2D MovementInput;

	AActor* CurrentAimHit;

	UMainUIWidget* MainUI;
	UStatusUIWidget* StatusUI;
	UStatusUIWidget* ExpandedStatusUI;
	UWidgetComponent* ExpandedStatusUIComponent;

	float CurrentForcedDilation;
	bool WantsDilate;

	float StandardCameraArmZOffset;
	float StandardCameraArmLength;
	float StandardCameraPitch;
	FVector StandardAimIndicatorScale;

	float PickupInfoTimer;
	FString PickupInfo;

	bool InventoryView;
	float InventoryViewFaceTimer;
	bool DropInventoryFocused;
	bool EquipInventoryFocused;

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
	void InputStartCheckStatus();
	void InputEndCheckStatus();
};
