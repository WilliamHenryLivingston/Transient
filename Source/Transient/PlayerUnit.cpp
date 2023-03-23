#include "PlayerUnit.h"

#include "Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerUnit::APlayerUnit() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->ColliderComponent->SetCollisionProfileName(FName("Player"), true);

	this->CurrentForcedDilation = 1.0f;

	this->CameraArmComponent = this->CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	this->CameraArmComponent->bInheritPitch = false;
	this->CameraArmComponent->bInheritRoll = false;
	this->CameraArmComponent->bInheritYaw = false;
	this->CameraArmComponent->TargetArmLength = 500.0f;
	this->CameraArmComponent->TargetOffset = FVector(0.0f, 0.0f, 300.0f);
	this->CameraArmComponent->SetupAttachment(this->RootComponent);

	this->CameraComponent = this->CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->CameraComponent->SetupAttachment(this->CameraArmComponent);
	this->CameraComponent->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	PlayerInputComponent->BindAxis("Forward", this, &APlayerUnit::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerUnit::InputRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerUnit::InputStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerUnit::InputStopFire);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerUnit::InputInteract);
	PlayerInputComponent->BindAction("Dilate", IE_Pressed, this, &APlayerUnit::InputStartDilate);
	PlayerInputComponent->BindAction("Dilate", IE_Released, this, &APlayerUnit::InputStopDilate);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerUnit::InputReload);
	PlayerInputComponent->BindAction("SlotA", IE_Pressed, this, &APlayerUnit::InputSwapWeaponA);
	PlayerInputComponent->BindAction("SlotB", IE_Pressed, this, &APlayerUnit::InputSwapWeaponB);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerUnit::InputStartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerUnit::InputEndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerUnit::InputJump);
}

void APlayerUnit::UnitDiscoverChildComponents() {	
	Super::UnitDiscoverChildComponents();

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents(StaticMeshComponents, true);
	for (int i = 0; i < StaticMeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = StaticMeshComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("AimIndicator")) this->AimIndicatorComponent = Check;
	}
}

// TODO: Move dilation.
void APlayerUnit::Tick(float DeltaTime) {
	this->GetWorld()->GetWorldSettings()->SetTimeDilation(this->CurrentForcedDilation);

	float RawDeltaTime = DeltaTime;
	DeltaTime = DeltaTime * (1.0f / this->CurrentForcedDilation);
	this->Animation->Script_TimeDilation = this->CurrentForcedDilation;

	Super::Tick(DeltaTime);
	
	if (this->WantsDilate && this->UnitDrainStamina(50.0f * RawDeltaTime)) {
		this->CurrentForcedDilation = FMath::Max(0.25f, this->CurrentForcedDilation - (RawDeltaTime * 3.0f));
		this->CameraComponent->PostProcessBlendWeight = FMath::Min(1.0f, this->CameraComponent->PostProcessBlendWeight + (RawDeltaTime * 3.0f));
	}
	else {
		this->CurrentForcedDilation = FMath::Min(1.0f, this->CurrentForcedDilation + (RawDeltaTime * 3.0f));
		this->CameraComponent->PostProcessBlendWeight = FMath::Max(0.0f, this->CameraComponent->PostProcessBlendWeight - (RawDeltaTime * 3.0f));
	}

	FHitResult MouseHit;
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, false, MouseHit);

	this->AimIndicatorComponent->SetWorldLocation(MouseHit.ImpactPoint);

	AActor* HitActor = MouseHit.GetActor();
	if (HitActor == this) {
		this->CurrentAimHit = nullptr;

		this->UnitUpdateTorsoPitch(0.0f);
	}
	else {
		this->CurrentAimHit = HitActor;

		if (HitActor == nullptr || HitActor->Tags.Contains(FName("Ground"))) {
			this->UnitUpdateTorsoPitch(0.0f);
		}
		else {
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(
				this->GetActorLocation() + this->ItemHolderGetWeaponOffset(),
				MouseHit.ImpactPoint
			);
			this->UnitUpdateTorsoPitch(LookRotation.Pitch);
		}

		this->UnitFaceTowards(MouseHit.ImpactPoint);
	}

	if (!this->MovementInput.IsZero()) {
		FVector2D AdjustedInput = this->MovementInput.GetSafeNormal() * 500.0f;

		FVector NewLocation = this->GetActorLocation();

		FVector CameraRight = this->CameraComponent->GetRightVector();
		CameraRight.Z = 0;

		FVector CameraForward = this->CameraComponent->GetForwardVector();
		CameraForward.Z = 0;

		NewLocation += CameraRight * AdjustedInput.X;
		NewLocation += CameraForward * AdjustedInput.Y;

		this->UnitMoveTowards(NewLocation);
	}
	
    this->UnitPostTick(DeltaTime);
}

void APlayerUnit::InputInteract() {
	AItemActor* AimedItem = nullptr;
	if (this->CurrentAimHit != nullptr) {
		AimedItem = Cast<AItemActor>(this->CurrentAimHit);

		// TODO: Out of reach check shouldnt be here.
		if (AimedItem != nullptr && (AimedItem->GetActorLocation() - this->GetActorLocation()).Size() > 150.0f) {
			AimedItem = nullptr;
		}
	}

	if (AimedItem != nullptr) this->UnitEquipItem(AimedItem);
}

// Input binds.
void APlayerUnit::InputJump() { this->UnitJump(); }
void APlayerUnit::InputReload() { this->UnitReload(); }
void APlayerUnit::InputStartFire() { this->UnitSetTriggerPulled(true); }
void APlayerUnit::InputStopFire() { this->UnitSetTriggerPulled(false); }
void APlayerUnit::InputStartCrouch() { this->UnitSetCrouched(true); }
void APlayerUnit::InputEndCrouch() { this->UnitSetCrouched(false); }

void APlayerUnit::InputForward(float AxisValue) {
	this->MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::InputRight(float AxisValue) {
	this->MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

// TODO: Need rework.
void APlayerUnit::InputSwapWeaponA() {
	if (this->UnitGetActiveWeaponSlot() != 0) {
		this->UnitSwapWeapons();
	}
}

void APlayerUnit::InputSwapWeaponB() {
	if (this->UnitGetActiveWeaponSlot() != 1) {
		this->UnitSwapWeapons();
	}
}

void APlayerUnit::InputStartDilate() {
	this->WantsDilate = true;
}

void APlayerUnit::InputStopDilate() {
	this->WantsDilate = false;
}
