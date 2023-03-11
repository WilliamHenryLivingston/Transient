#include "PlayerUnit.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"

APlayerUnit::APlayerUnit() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->CameraComponent = this->CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->CameraComponent->SetupAttachment(this->RootComponent);
	this->CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	this->CameraComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::BeginPlay() {
	Super::BeginPlay();
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	PlayerInputComponent->BindAxis("Forward", this, &APlayerUnit::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerUnit::InputRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerUnit::InputStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerUnit::InputStopFire);
}

void APlayerUnit::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FHitResult MouseHit = FHitResult();
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

	this->UnitFaceTowards(MouseHit.ImpactPoint);

	if (!this->MovementInput.IsZero())
	{
		FVector2D AdjustedInput = this->MovementInput.GetSafeNormal() * 20.0f;

		FVector NewLocation = this->GetActorLocation();

		NewLocation += this->CameraComponent->GetRightVector() * AdjustedInput.X;
		NewLocation += this->CameraComponent->GetUpVector() * AdjustedInput.Y;

		this->UnitMoveTowards(NewLocation, DeltaTime);
	}
}

void APlayerUnit::OnUnitFace(FRotator Rotation) {
	this->CameraComponent->SetRelativeRotation(FRotator(-90.0f, -Rotation.Yaw, 0.0f));
}

void APlayerUnit::InputStartFire() {
	this->UnitSetTriggerPulled(true);
}

void APlayerUnit::InputStopFire() {
	this->UnitSetTriggerPulled(false);
}

void APlayerUnit::InputForward(float AxisValue) {
	this->MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::InputRight(float AxisValue) {
	this->MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}
