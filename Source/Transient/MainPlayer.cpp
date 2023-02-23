// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	if (!MovementInput.IsZero())
	{
		//Scale our movement input axis values by 100 units per second
		MovementInput = MovementInput.GetSafeNormal() * 100.0f;
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime;
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::InputForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::InputRight);
}


void AMainPlayer::InputForward(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AMainPlayer::InputRight(float AxisValue)
{
	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}
