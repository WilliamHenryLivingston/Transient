// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider")); 
	UBoxComponent* Collider = (UBoxComponent*)RootComponent;

	Collider->SetCollisionProfileName(FName("Pawn"), true);
	
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	VisibleComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Meshes/cube.cube'"));
	VisibleComponent->SetStaticMesh(MeshAsset.Object);

	VisibleComponent->SetRelativeScale3D(FVector(30.0f, 30.0f, 30.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	CameraComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("playerpawn spawned"));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult MouseHit = FHitResult();
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

	FRotator CurrentRotation = GetActorRotation();
	FVector CurrentLocation = GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, MouseHit.ImpactPoint);

	FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

	CameraComponent->SetRelativeRotation(FRotator(-90.0f, -NewRotation.Yaw, 0.0f));
	SetActorRotation(NewRotation);

	if (!MovementInput.IsZero())
	{
		MovementInput = MovementInput.GetSafeNormal() * Speed;

		FVector PrevLocation = GetActorLocation();
		FVector NewLocation = GetActorLocation();

		NewLocation += CameraComponent->GetRightVector() * MovementInput.X * DeltaTime;
		NewLocation += CameraComponent->GetUpVector() * MovementInput.Y * DeltaTime;

		FVector Move = NewLocation - PrevLocation;
		FVector ActorForward = GetActorForwardVector();

		float Angle = acos(Move.Dot(ActorForward) / (Move.Length() * ActorForward.Length()));
		// TODO cleanup
		if (Angle > StrafeConeAngle)
		{
			NewLocation -= Move * StrafeModifier;
		}

		SetActorLocation(NewLocation);
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Forward", this, &APlayerPawn::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerPawn::InputRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerPawn::InputFire);
}

void APlayerPawn::InputFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("fire"));
}

void APlayerPawn::InputForward(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerPawn::InputRight(float AxisValue)
{
	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}
