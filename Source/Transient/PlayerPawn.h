// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"

#include "PlayerPawn.generated.h"

UCLASS()
class TRANSIENT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY(EditAnywhere)
	float Speed;
	
	UPROPERTY(EditAnywhere)
	float StrafeModifier;

	UPROPERTY(EditAnywhere)
	float StrafeConeAngle;

	UPROPERTY(EditAnywhere)
	USceneComponent* VisibleComponent;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	FVector2D MovementInput;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void InputFire();
	UFUNCTION()
	void InputForward(float AxisValue);
	UFUNCTION()
	void InputRight(float AxisValue);
};
