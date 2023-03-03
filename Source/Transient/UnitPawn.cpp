#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AUnitPawn::AUnitPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	UBoxComponent* Collider = (UBoxComponent*)RootComponent;

	Collider->SetSimulatePhysics(true);
	Collider->SetLinearDamping(20.0f);
	Collider->BodyInstance.bLockXRotation = true;
	Collider->BodyInstance.bLockYRotation = true;
	Collider->SetEnableGravity(true);
	Collider->SetCollisionProfileName(FName("Pawn"), true);

	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	VisibleComponent->SetupAttachment(RootComponent);
	VisibleComponent->SetRelativeScale3D(FVector(30.0f, 30.0f, 30.0f));
}

void AUnitPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AUnitPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnitPawn::UnitMoveTowards(FVector Target, float DeltaTime)
{
	FVector Move = (Target - GetActorLocation()).GetSafeNormal() * Speed * DeltaTime;
	FVector ActorForward = GetActorForwardVector();

	float Angle = acos(Move.Dot(ActorForward) / (Move.Length() * ActorForward.Length()));
	if (Angle > StrafeConeAngle)
	{
		Target *= StrafeModifier;
	}

	SetActorLocation(GetActorLocation() + Move);
}

void AUnitPawn::UnitFaceTowards(FVector Target)
{
	FRotator CurrentRotation = GetActorRotation();
	FVector CurrentLocation = GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, Target);

	FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

	OnUnitFace(NewRotation);
	SetActorRotation(NewRotation);
}

void AUnitPawn::OnUnitFace(FRotator Rotation)
{

}