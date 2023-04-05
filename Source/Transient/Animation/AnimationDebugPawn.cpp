// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AnimationDebugPawn.h"

AAnimationDebugPawn::AAnimationDebugPawn() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAnimationDebugPawn::BeginPlay() {
	Super::BeginPlay();
}

void AAnimationDebugPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->SetActorLocation(
		this->GetActorLocation() +
		this->GetActorRotation().RotateVector(this->MovementInput * this->Speed * DeltaTime)
	);
}

void AAnimationDebugPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Forward", this, &AAnimationDebugPawn::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &AAnimationDebugPawn::InputRight);
}

void AAnimationDebugPawn::InputForward(float AxisValue) {
	this->MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AAnimationDebugPawn::InputRight(float AxisValue) {
	this->MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}