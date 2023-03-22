#include "PlayerUnit.h"

#include "Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerUnit::APlayerUnit() {
	this->PrimaryActorTick.bCanEverTick = true;
	
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

void APlayerUnit::BeginPlay() {
	Super::BeginPlay();
	
	// Discover child mesh hosts. TODO copy pasted
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents(StaticMeshComponents, true);
	for (int i = 0; i < StaticMeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = StaticMeshComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("AimIndicator")) {
			this->AimIndicatorComponent = Check;
		}
	}
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
}

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

	FHitResult MouseHit = FHitResult();
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

	AActor* HitActor = MouseHit.GetActor();
	this->AimIndicatorComponent->SetWorldLocation(MouseHit.ImpactPoint);
	if (HitActor != this) {
		if (HitActor == nullptr || HitActor->Tags.Contains(FName("Ground"))) {
			this->RootPitch = FMath::Max(0.0f, this->RootPitch - (RawDeltaTime * 90.0f));
		}
		else {
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation() + this->WeaponOffset, MouseHit.ImpactPoint);
			if (LookRotation.Pitch > this->RootPitch) {
				this->RootPitch = FMath::Min(LookRotation.Pitch, this->RootPitch + (RawDeltaTime * 90.0f));
			}
			else {
				this->RootPitch = FMath::Max(LookRotation.Pitch, this->RootPitch - (RawDeltaTime * 90.0f));
			}
		}

		this->UnitFaceTowards(MouseHit.ImpactPoint);
	}
	else {
		this->RootPitch = FMath::Max(0.0f, this->RootPitch - (RawDeltaTime * 5.0f));
	}

	if (!this->MovementInput.IsZero()) {
		FVector2D AdjustedInput = this->MovementInput.GetSafeNormal() * 20.0f;

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
	TArray<AItemActor*> NearbyItems = AItemActor::ItemsGetNearby(this->GetActorLocation(), 100.0f);

	if (NearbyItems.Num() > 0) {
		this->UnitEquipItem(NearbyItems[0]);
	}
	else if (this->WeaponItem != nullptr) {
		this->UnitEquipWeapon(nullptr);
	}
	else {
		this->UnitEquipArmor(nullptr);
	}
}

void APlayerUnit::InputReload() {
	this->UnitReload();
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

void APlayerUnit::InputStartDilate() {
	this->WantsDilate = true;
}

void APlayerUnit::InputStopDilate() {
	this->WantsDilate = false;
}

void APlayerUnit::InputSwapWeaponA() {
	if (this->ActiveWeaponSlot != 0) {
		this->UnitSwapWeapons();
	}
}

void APlayerUnit::InputSwapWeaponB() {
	if (this->ActiveWeaponSlot != 1) {
		this->UnitSwapWeapons();
	}
}

void APlayerUnit::InputStartCrouch() {
	this->Crouching = true;
}

void APlayerUnit::InputEndCrouch() {
	this->Crouching = false;
}