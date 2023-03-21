#include "PlayerUnit.h"

#include "Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

void APlayerUnit::Tick(float DeltaTime) {
	DeltaTime = DeltaTime * (1.0f / this->CurrentForcedDilation);

	Super::Tick(DeltaTime);

	FHitResult MouseHit = FHitResult();
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

	this->UnitFaceTowards(MouseHit.ImpactPoint);

	this->AimIndicatorComponent->SetWorldLocation(MouseHit.ImpactPoint);

	if (!this->MovementInput.IsZero())
	{
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
	TArray<AItemActor*> NearbyItems = AItemActor::ItemsGetNearby(this->GetActorLocation(), 100);

	bool TookAction = false;
	for (int i = 0; i < NearbyItems.Num(); i++) {
		AItemActor* Check = NearbyItems[i];

		AWeaponItem* AsWeapon = Cast<AWeaponItem>(Check);
		if (AsWeapon != nullptr) {
			this->UnitEquipWeapon(AsWeapon);
			return;
		}

		AArmorItem* AsArmor = Cast<AArmorItem>(Check);
		if (AsArmor != nullptr) {
			this->UnitEquipArmor(AsArmor);
			return;
		}
	}
	
	if (this->WeaponItem != nullptr) {
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
	this->GetWorld()->GetWorldSettings()->SetTimeDilation(0.25);
	this->CurrentForcedDilation = 0.25;
}

void APlayerUnit::InputStopDilate() {
	this->GetWorld()->GetWorldSettings()->SetTimeDilation(1.0);
	this->CurrentForcedDilation = 1.0;
}
