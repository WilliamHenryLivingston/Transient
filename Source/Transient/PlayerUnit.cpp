#include "PlayerUnit.h"

#include "Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Environment/InteractiveActor.h"
#include "UnitSlotColliderComponent.h"

APlayerUnit::APlayerUnit() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	PlayerInputComponent->BindAxis("Forward", this, &APlayerUnit::InputForward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerUnit::InputRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerUnit::InputStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerUnit::InputStopFire);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerUnit::InputInteract);
	PlayerInputComponent->BindAction("Exert", IE_Pressed, this, &APlayerUnit::InputStartExert);
	PlayerInputComponent->BindAction("Exert", IE_Released, this, &APlayerUnit::InputEndExert);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerUnit::InputReload);
	PlayerInputComponent->BindAction("SlotA", IE_Pressed, this, &APlayerUnit::InputEquipSlotA);
	PlayerInputComponent->BindAction("SlotB", IE_Pressed, this, &APlayerUnit::InputEquipSlotB);
	PlayerInputComponent->BindAction("SlotC", IE_Pressed, this, &APlayerUnit::InputEquipSlotC);
	PlayerInputComponent->BindAction("SlotD", IE_Pressed, this, &APlayerUnit::InputEquipSlotD);
	PlayerInputComponent->BindAction("SlotE", IE_Pressed, this, &APlayerUnit::InputEquipSlotE);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerUnit::InputStartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerUnit::InputEndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerUnit::InputJump);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APlayerUnit::InputDropActive);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &APlayerUnit::InputEnterInventory);
	PlayerInputComponent->BindAction("Inventory", IE_Released, this, &APlayerUnit::InputExitInventory);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerUnit::InputStartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerUnit::InputEndAim);
	PlayerInputComponent->BindAction("CheckStatus", IE_Pressed, this, &APlayerUnit::InputStartCheckStatus);
	PlayerInputComponent->BindAction("CheckStatus", IE_Released, this, &APlayerUnit::InputEndCheckStatus);
}

void APlayerUnit::BeginPlay() {
	Super::BeginPlay();

	this->CurrentForcedDilation = 1.0f;

	this->CameraArmComponent = this->FindComponentByClass<USpringArmComponent>();
	this->CameraComponent = this->FindComponentByClass<UCameraComponent>();

	this->ColliderComponent->SetCollisionProfileName(FName("Player"), true);

	// TODO: Template helper for this.
	TArray<UStaticMeshComponent*> MeshComponents;
	this->GetComponents(MeshComponents, true);
	for (int i = 0; i < MeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = MeshComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("AimIndicator")) this->AimIndicatorComponent = Check;
	}

	this->StandardCameraArmLength = this->CameraArmComponent->TargetArmLength;
	this->StandardCameraArmZOffset = this->CameraArmComponent->TargetOffset.Z;
	this->StandardCameraPitch = this->CameraComponent->GetRelativeRotation().Pitch;
	this->StandardAimIndicatorScale = this->AimIndicatorComponent->GetRelativeScale3D();

	// First zoom is bugged without this.
	FRotator InitialRotation = this->CameraComponent->GetRelativeRotation();
	InitialRotation.Pitch -= 0.1f;
	this->CameraComponent->SetRelativeRotation(InitialRotation);

	// Setup UIs.
	TArray<UWidgetComponent*> WidgetComponents;
	this->GetComponents(WidgetComponents, true);
	for (int i = 0; i < WidgetComponents.Num(); i++) {
		UWidgetComponent* Check = WidgetComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("StatusWidget")) this->StatusUI = Cast<UStatusUIWidget>(Check->GetUserWidgetObject());
		else if (Name.Equals("ExpandedStatusWidget")) {
			this->ExpandedStatusUIComponent = Check;
			this->ExpandedStatusUI = Cast<UStatusUIWidget>(Check->GetUserWidgetObject());
		}
	}

	this->MainUI = Cast<UMainUIWidget>(CreateWidget<UUserWidget>(this->GetWorld(), this->MainUIType));
	this->MainUI->AddToViewport(1000);
}

// TODO: Move dilation. Also this is a complete mess.
void APlayerUnit::Tick(float DeltaTime) {
	this->GetWorld()->GetWorldSettings()->SetTimeDilation(this->CurrentForcedDilation);

	float RawDeltaTime = DeltaTime;
	DeltaTime = DeltaTime * (1.0f / this->CurrentForcedDilation);

	Super::Tick(DeltaTime);
	this->AnimationScale = this->CurrentForcedDilation;

	FRotator CameraRotation = this->CameraComponent->GetRelativeRotation();

	float CameraMoveDelta = this->InventoryViewCameraLerpRate * DeltaTime;
	float CameraAngleDelta = this->InventoryViewCameraPitchLerpRate * DeltaTime;

	this->UnitImmobilize(this->InventoryView);
	this->IgnoreTorsoYaw = this->InventoryView;

	bool ResetAim = true;

	if (this->InventoryView) {
		this->Aiming = false;
		this->UnitSetTriggerPulled(false);

		this->AimIndicatorComponent->SetRelativeScale3D(this->StandardAimIndicatorScale * 0.4f);
		this->CameraArmComponent->TargetArmLength = FMath::Max(
			this->InventoryViewDistance,
			this->CameraArmComponent->TargetArmLength - CameraMoveDelta
		);
		this->CameraArmComponent->TargetOffset.Z = FMath::Max(
			0.0f,
			this->CameraArmComponent->TargetOffset.Z - CameraMoveDelta
		);

		float NextCameraPitch = CameraRotation.Pitch + CameraAngleDelta;
		if (NextCameraPitch > 0.0f) NextCameraPitch = 0.0f;

		CameraRotation.Pitch = NextCameraPitch;
	}
	else {
		this->AimIndicatorComponent->SetRelativeScale3D(this->StandardAimIndicatorScale);

		this->CameraArmComponent->TargetArmLength = FMath::Min(
			this->StandardCameraArmLength,
			this->CameraArmComponent->TargetArmLength + CameraMoveDelta
		);
		this->CameraArmComponent->TargetOffset.Z = FMath::Min(
			this->StandardCameraArmZOffset,
			this->CameraArmComponent->TargetOffset.Z + CameraMoveDelta
		);

		float NextCameraPitch = CameraRotation.Pitch - CameraAngleDelta;
		if (NextCameraPitch < this->StandardCameraPitch) NextCameraPitch = this->StandardCameraPitch;

		CameraRotation.Pitch = NextCameraPitch;
	}
	this->CameraComponent->SetRelativeRotation(CameraRotation);
	
	if (this->UnitIsExerted() && this->UnitGetItemByName(TEXT("time dilator")) != nullptr) {
		this->CurrentForcedDilation = FMath::Max(0.25f, this->CurrentForcedDilation - (RawDeltaTime * 3.0f));
		this->CameraComponent->PostProcessSettings.SceneFringeIntensity = FMath::Min(this->SlowEffectStrength, this->CameraComponent->PostProcessSettings.SceneFringeIntensity + (RawDeltaTime * 3.0f));
	}
	else {
		this->CurrentForcedDilation = FMath::Min(1.0f, this->CurrentForcedDilation + (RawDeltaTime * 3.0f));
		this->CameraComponent->PostProcessSettings.SceneFringeIntensity = FMath::Max(0.0f, this->CameraComponent->PostProcessSettings.SceneFringeIntensity - (RawDeltaTime * 3.0f));
	}

	FHitResult MouseHit;
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(
		this->InventoryView ? ETraceTypeQuery::TraceTypeQuery3 : ETraceTypeQuery::TraceTypeQuery4,
		false,
		MouseHit
	);

	this->AimIndicatorComponent->SetWorldLocation(MouseHit.ImpactPoint);

	this->ExpandedStatusUI->Script_KineticHealth = this->StatusUI->Script_KineticHealth = this->KineticHealth / this->MaxKineticHealth;
	this->ExpandedStatusUI->Script_Energy = this->StatusUI->Script_Energy = this->Energy / this->MaxEnergy;
	this->ExpandedStatusUI->Script_Stamina = this->StatusUI->Script_Stamina = this->Stamina / this->MaxStamina;
	this->ExpandedStatusUIComponent->SetVisibility(this->CheckingStatus);
	if (this->CheckingStatus) {
		this->ExpandedStatusUIComponent->SetWorldRotation(
			UKismetMathLibrary::FindLookAtRotation(
				this->ExpandedStatusUIComponent->GetComponentLocation(),
				this->CameraComponent->GetComponentLocation()
			)
		);
	}

	this->MainUI->Script_CurrentItemDescriptor = TEXT("");
	this->ForceArmsEmptyAnimation = this->InventoryView;
	if (this->InventoryView) {
		this->UnitUpdateTorsoPitch(0.0f);

		bool DropCurrent = this->DropInventoryFocused;
		this->DropInventoryFocused = false;
		bool EquipCurrent = this->EquipInventoryFocused;
		this->EquipInventoryFocused = false;

		FRotator CurrentRotation = this->GetActorRotation();
		if (this->InventoryViewFaceTimer > 0.0f) {
			this->InventoryViewFaceTimer -= DeltaTime;

			this->UnitFaceTowards(this->CameraComponent->GetComponentLocation());
		}
		else {
			CurrentRotation.Yaw += this->MovementInput.X * DeltaTime * this->InventoryViewCameraYawLerpRate;

			this->SetActorRotation(CurrentRotation);
		}

		UPrimitiveComponent* HitComponent = MouseHit.GetComponent();
		if (HitComponent != nullptr) {
			UUnitSlotColliderComponent* AsSlotCollider = Cast<UUnitSlotColliderComponent>(HitComponent);

			AItemActor* TargetedItem = nullptr;

			if (AsSlotCollider != nullptr) {
				UUnitSlotComponent* HitSlot = Cast<UUnitSlotComponent>(AsSlotCollider->ParentSlot);
				TargetedItem = HitSlot->SlotGetContent();
			}
			else if (HitComponent->GetName().Equals(TEXT("InvViewActive"))) {
				TargetedItem = this->UnitGetActiveItem();
				if (TargetedItem != nullptr && TargetedItem->EquipAltHand) {
					TargetedItem = nullptr;
				}
			}
			else if (HitComponent->GetName().Equals(TEXT("InvViewAltActive"))) {
				TargetedItem = this->UnitGetActiveItem();
				if (TargetedItem != nullptr && !TargetedItem->EquipAltHand) {
					TargetedItem = nullptr;
				}
			}
			else if (HitComponent->GetName().Equals(TEXT("InvViewArmor"))) {
				TargetedItem = this->UnitGetArmor();
			}
			else if (HitComponent->GetName().Equals(TEXT("InvViewSelf"))) {
				AItemActor* AsItem = Cast<AItemActor>(MouseHit.GetActor());
				
				if (this->UnitHasItem(AsItem)) TargetedItem = AsItem;
			}
			
			if (TargetedItem != nullptr) {
				this->MainUI->Script_CurrentItemDescriptor = TargetedItem->ItemGetDescriptorString();

				if (DropCurrent) {
					this->OverrideArmsState = true;
					this->UnitDropItem(TargetedItem);
					this->OverrideArmsState = false;
					this->UnitPlayInteractAnimation();
				}
				else if (EquipCurrent) {
					this->OverrideArmsState = true;
					this->UnitEquipItem(TargetedItem);
					this->OverrideArmsState = false;
					this->UnitPlayInteractAnimation();
				}
			}
		}
	}
	else {
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
					this->AimRootComponent->GetComponentLocation(),
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
	}

	float AimDelta = this->AimSpeed * DeltaTime;
	if (this->Aiming) {
		float MouseSX, MouseSY;
		this->GetWorld()->GetFirstPlayerController()->GetMousePosition(MouseSX, MouseSY);
		int SSizeX, SSizeY;
		this->GetWorld()->GetFirstPlayerController()->GetViewportSize(SSizeX, SSizeY);
		MouseSX /= SSizeX;
		MouseSY /= SSizeY;

		if (MouseSX < this->AimPadding && this->AimCameraOffset.Y > -this->AimMaxDistance) {
			this->AimCameraOffset.Y -= AimDelta;
		}
		if (MouseSX > (1.0f - this->AimPadding) && this->AimCameraOffset.Y < this->AimMaxDistance) {
			this->AimCameraOffset.Y += AimDelta; 
		}
		if (MouseSY < this->AimPadding && this->AimCameraOffset.X < this->AimMaxDistance) {
			this->AimCameraOffset.X += AimDelta;
		}
		if (MouseSY > (1.0f - this->AimPadding) && this->AimCameraOffset.X > -this->AimMaxDistance) {
			this->AimCameraOffset.X -= AimDelta; 
		}
	}
	else {
		AimDelta *= 5.0f;
		if (this->AimCameraOffset.X > 0.0f) this->AimCameraOffset.X -= AimDelta;
		if (this->AimCameraOffset.X < 0.0f) this->AimCameraOffset.X += AimDelta;
		if (this->AimCameraOffset.Y > 0.0f) this->AimCameraOffset.Y -= AimDelta;
		if (this->AimCameraOffset.Y < 0.0f) this->AimCameraOffset.Y += AimDelta;
	}
	this->CameraArmComponent->SetWorldLocation(this->GetActorLocation() + this->AimCameraOffset);
	
    this->UnitPostTick(DeltaTime);

	bool ForceInventoryPose = (
		this->InventoryView &&
		this->Animation->Script_ArmsModifier != EUnitAnimArmsModifier::Reload &&
		this->Animation->Script_ArmsModifier != EUnitAnimArmsModifier::Use &&
		this->Animation->Script_ArmsModifier != EUnitAnimArmsModifier::Interact
	);
	if (ForceInventoryPose) {
		this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::Inventory;
	}
}

// TODO: Out of reach checks shouldn't be here.
void APlayerUnit::InputInteract() {
	if (this->CurrentAimHit == nullptr) return;
	
	float Distance = (this->CurrentAimHit->GetActorLocation() - this->GetActorLocation()).Size();

	AItemActor* AimedItem = Cast<AItemActor>(this->CurrentAimHit);
	if (Distance > this->TakeReach) AimedItem = nullptr;

	if (AimedItem != nullptr) {
		this->UnitTakeItem(AimedItem);
		return;
	}

	if (this->CurrentAimHit->IsA(AInteractiveActor::StaticClass())) {
		this->UnitInteractWith(this->CurrentAimHit);
		return;
	}
}

// Input binds.
void APlayerUnit::InputJump() { this->UnitJump(); }
void APlayerUnit::InputReload() { this->UnitReload(); }
void APlayerUnit::InputStopFire() { this->UnitSetTriggerPulled(false); }
void APlayerUnit::InputStartCrouch() { this->UnitSetCrouched(true); }
void APlayerUnit::InputEndCrouch() { this->UnitSetCrouched(false); }
void APlayerUnit::InputEquipSlotA() { this->UnitEquipFromSlot(0); }
void APlayerUnit::InputEquipSlotB() { this->UnitEquipFromSlot(1); }
void APlayerUnit::InputEquipSlotC() { this->UnitEquipFromSlot(2); }
void APlayerUnit::InputEquipSlotD() { this->UnitEquipFromSlot(3); }
void APlayerUnit::InputEquipSlotE() { this->UnitEquipFromSlot(4); }
void APlayerUnit::InputDropActive() { this->UnitDropActiveItem(); }

void APlayerUnit::InputStartAim() {
	if (this->InventoryView) {
		this->DropInventoryFocused = true;
	}
	else {
		this->Aiming = true;
	}
}

void APlayerUnit::InputEndAim() {
	this->Aiming = false;
}

void APlayerUnit::InputStartFire() {
	if (this->InventoryView) {
		this->EquipInventoryFocused = true;
	}
	else if (this->UnitGetActiveWeapon() != nullptr) {
		this->UnitSetTriggerPulled(true);
	}
	else {
		this->UnitUseActiveItem(this->CurrentAimHit);
	}
}

void APlayerUnit::InputForward(float AxisValue) {
	this->MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::InputRight(float AxisValue) {
	this->MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerUnit::InputStartExert() {
	this->UnitSetExerted(true);
}

void APlayerUnit::InputEndExert() {
	this->UnitSetExerted(false);
}

void APlayerUnit::InputEnterInventory() {
	this->InventoryView = true;
	this->InventoryViewFaceTimer = 0.5f;
}

void APlayerUnit::InputExitInventory() {
	this->InventoryView = false;
}

void APlayerUnit::InputStartCheckStatus() {
	this->UnitSetCheckingStatus(true);
}

void APlayerUnit::InputEndCheckStatus() {
	this->UnitSetCheckingStatus(false);
}