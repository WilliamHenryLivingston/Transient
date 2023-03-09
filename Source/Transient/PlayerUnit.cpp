#include "PlayerUnit.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"

APlayerUnit::APlayerUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	CameraComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("..."));
}

void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult MouseHit = FHitResult();
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

	UnitFaceTowards(MouseHit.ImpactPoint);

	if (!MovementInput.IsZero())
	{
		MovementInput = MovementInput.GetSafeNormal() * 20.0f;

		FVector NewLocation = GetActorLocation();

		NewLocation += CameraComponent->GetRightVector() * MovementInput.X;
		NewLocation += CameraComponent->GetUpVector() * MovementInput.Y;

		UnitMoveTowards(NewLocation, DeltaTime);
	}
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Forward", this, &APlayerUnit::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerUnit::InputRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerUnit::InputFire);
}

void APlayerUnit::InputFire()
{
	UnitFire();
}

void APlayerUnit::InputForward(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::InputRight(float AxisValue)
{
	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::OnUnitFace(FRotator Rotation)
{
	CameraComponent->SetRelativeRotation(FRotator(-90.0f, -Rotation.Yaw, 0.0f));
}