// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Debug.h"
#include "PlayerUnit.h"
#include "Items/ProjectileWeapon.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();

	this->ArmsActionMoveLock = false;

	this->JumpTimer = -1.0f; // TODO: Janky.
	
	this->RigComponent = this->FindComponentByClass<ULegIKSkeletonComponent>();
	this->Animation = Cast<UUnitAnimInstance>(this->RigComponent->GetAnimInstance());
	this->AudioComponent = this->FindComponentByClass<UAudioComponent>();

	TArray<UShapeComponent*> ShapeComponents;
	this->GetComponents(ShapeComponents, true);
	for (int i = 0; i < ShapeComponents.Num(); i++) {
		UShapeComponent* Check = ShapeComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("RootCollider")) this->ColliderComponent = Check;
	}
	
	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("ActiveItemHost")) this->ActiveItemHostComponent = Check;
		else if (Name.Equals("ActiveItemAltHost")) this->ActiveItemAltHostComponent = Check;
		else if (Name.Equals("AimRoot")) this->AimRootComponent = Check;
	}

	this->BaseColliderVerticalScale = this->ColliderComponent->GetRelativeScale3D().Z;
	this->BaseRigScale = this->RigComponent->GetRelativeScale3D();
	this->BaseRigVerticalOffset = this->RigComponent->GetRelativeLocation().Z;

	this->UnitDiscoverDynamicChildComponents();

	// Initialize inventory.
    this->OverrideArmsState = true;

	if (this->ActiveItem != nullptr) this->UnitTakeItem(this->ActiveItem);
	if (this->ArmorItem != nullptr) this->UnitTakeItem(this->ArmorItem);

    for (int i = 0; i < this->AutoSpawnInitialItems.Num(); i++) {
        AItemActor* Spawned = this->GetWorld()->SpawnActor<AItemActor>(
            this->AutoSpawnInitialItems[i],
            this->GetActorLocation(),
            this->GetActorRotation(),
            FActorSpawnParameters()
        );
        if (Spawned == nullptr) continue;

        this->UnitTakeItem(Spawned);
    }

	this->OverrideArmsState = false;
}

void AUnitPawn::UnitDiscoverDynamicChildComponents() {
	TArray<UUnitSlotComponent*> SlotComponents;
	this->GetComponents(SlotComponents, true);

	TArray<AActor*> Attached;
	this->GetAttachedActors(Attached, true);

	for (int i = 0; i < Attached.Num(); i++) {
		AActor* Check = Attached[i];

		TArray<UUnitSlotComponent*> IncludedComponents;
		Check->GetComponents(IncludedComponents, true);

		SlotComponents.Append(IncludedComponents);
	}

	this->Slots = SlotComponents;
}

void AUnitPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->AudioComponent->PitchMultiplier = this->GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	if (this->Exerted && (!this->UnitDrainStamina(this->ExertedStaminaDrain * DeltaTime))) this->Exerted = false;

	// Clear child class targeting for their tick.
	this->TargetTorsoPitch = this->TorsoPitch; // TODO: Should be 0?
	this->HasFaceTarget = false;
	this->AnimationScale = 1.0f;

	if (this->JumpTimer < 0.0f) {
		this->HasMoveTarget = false;
	}
}

void AUnitPawn::UnitPostTick(float DeltaTime) {
	bool LowPower = this->Stamina <= 1.0f && this->Energy <= 1.0f;
	if (LowPower) {
		this->AnimationScale *= 4.0f;
	}
	
	if (this->CurrentInteractActor != nullptr) {
		float InteractDelta = DeltaTime;
		if (LowPower) InteractDelta *= 0.25f;

		this->Animation->Script_ArmsInteractTarget = this->CurrentInteractActor->InteractAnimation;
		this->CurrentInteractActor->InteractTimer += InteractDelta;
		this->TargetTorsoPitch = UKismetMathLibrary::FindLookAtRotation(
			this->AimRootComponent->GetComponentLocation(),
			this->CurrentInteractActor->InteractLookTargetComponent->GetComponentLocation()
		).Pitch;
		this->FaceTarget = this->CurrentInteractActor->GetActorLocation();
		this->HasFaceTarget = true;
		this->Immobilized = true;
	}
	else {
		this->Animation->Script_ArmsInteractTarget = EUnitAnimArmsInteractTarget::None;
	}

	// Update torso pitch.
	if (this->TorsoPitch < this->TargetTorsoPitch) {
		this->TorsoPitch = FMath::Min(this->TargetTorsoPitch, this->TorsoPitch + (this->LookPitchSpeed * DeltaTime));
	}
	else {
		this->TorsoPitch = FMath::Max(this->TargetTorsoPitch, this->TorsoPitch - (this->LookPitchSpeed * DeltaTime));
	}
	this->TorsoPitch = FMath::Clamp<float>(this->TorsoPitch, -this->LookPitchLimit, this->LookPitchLimit);
	this->Animation->Script_TorsoPitch = -this->TorsoPitch;

	// Stats update.
	if (this->StaminaRegenTimer > 0.0f) {
		this->StaminaRegenTimer -= DeltaTime;
	}
	else if (this->Stamina < this->MaxStamina && this->UnitDrainEnergy(this->StaminaRegen * 0.25f * DeltaTime)) {
		this->Stamina = FMath::Min(this->MaxStamina, this->Stamina + (this->StaminaRegen * DeltaTime));
	}

	// Timers and animation updates.
	// ...Arms mode.
	bool UseItemTorsoYaw = false;
	if (this->ActiveItem == nullptr) {
		this->Animation->Script_ArmsState = EUnitAnimArmsState::Empty;
	}
	else {
		this->ActiveItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)); // TODO: What the fuck?
		this->Animation->Script_ArmsState = this->ActiveItem->EquippedAnimArmsMode;

		// Last check is because reload is two anim cycles.
		if (!this->IgnoreTorsoYaw && this->ArmsAnimationTimer <= 0.5f && this->LoadingMagazine == nullptr) UseItemTorsoYaw = true;
	}
	if (this->ForceArmsEmptyAnimation) {
		this->Animation->Script_ArmsState = EUnitAnimArmsState::Empty;
	}

	float TorsoYawDelta = this->LookPitchSpeed * DeltaTime;
	if (UseItemTorsoYaw) {
		this->Animation->Script_TorsoYaw = FMath::Min(
			this->ActiveItem->EquippedTorsoYaw,
			this->Animation->Script_TorsoYaw + TorsoYawDelta
		);
	}
	else {
		this->Animation->Script_TorsoYaw = FMath::Max(
			0.0f,
			this->Animation->Script_TorsoYaw - TorsoYawDelta
		);
	}

	// ...Arms modifier (animation).
	if (this->CheckingStatus) {
		this->ArmsAnimationTimer = 10.0f;
		this->ArmsAnimation = EUnitAnimArmsModifier::CheckStatus;
	}
	else if (this->ArmsAnimation == EUnitAnimArmsModifier::CheckStatus) {
		this->ArmsAnimationTimer = 0.0f;
	}

	this->ArmsAnimationCooldownTimer -= DeltaTime;
	if (this->ArmsAnimationTimer > 0.0f) {
		float TimerRate = DeltaTime;
		if (LowPower) TimerRate *= 0.25f;

		this->ArmsAnimationTimer -= TimerRate;
		this->Animation->Script_ArmsModifier = this->ArmsAnimation;

		if (this->ArmsAnimationTimer < 0.0f) {
			this->ArmsAnimationCooldownTimer = 0.1f;

			if (this->ArmsAnimationThen != nullptr) {
				void (AUnitPawn::*Callback)() = this->ArmsAnimationThen;
				this->ArmsAnimationThen = nullptr;

				(this->*(Callback))();
			}
		}
	}
	else {
		AWeaponItem* CurrentWeapon = this->UnitGetActiveWeapon();
		if (CurrentWeapon != nullptr && CurrentWeapon->WeaponGetTriggerPulled() && !CurrentWeapon->WeaponEmpty()) {
			this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::Fire;
		}
		else {
			this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::None;
		}
	}

	// Movement update.
	FLegIKDynamics LegIK;

	if (this->Crouching || this->Immobilized || LowPower) {
		LegIK.StepDistanceCoef *= 0.25f;
	}

	FVector CurrentLocation = this->GetActorLocation();	

	bool ActiveJump = this->JumpTimer > 0.0f;
	if (ActiveJump) this->JumpTimer -= DeltaTime;

	EUnitAnimLegsState LegsState = EUnitAnimLegsState::None;
	EUnitAnimLegsModifier LegsModifier = EUnitAnimLegsModifier::None;

	FVector ColliderScale = this->ColliderComponent->GetRelativeScale3D();
	ColliderScale.Z = this->BaseColliderVerticalScale;
	FVector RigLocation = this->RigComponent->GetRelativeLocation();
	RigLocation.Z = this->BaseRigVerticalOffset;
	FVector RigScale = this->BaseRigScale;
	if (this->Crouching) {
		LegsState = EUnitAnimLegsState::Crouch;
		ColliderScale.Z *= this->CrouchVerticalShrink;
		RigLocation.Z += this->CrouchVerticalTranslate;
		RigScale.Z *= this->BaseColliderVerticalScale / ColliderScale.Z;
		LegIK.BodyBaseCoef *= this->CrouchVerticalShrink;
	}
	this->ColliderComponent->SetRelativeScale3D(ColliderScale);
	this->RigComponent->SetRelativeLocation(RigLocation);
	this->RigComponent->SetRelativeScale3D(RigScale);

	if (this->HasMoveTarget && !this->Immobilized && !this->ArmsActionMoveLock) {
		float Speed = this->MoveSpeed;
		if (LowPower) Speed *= 0.25f;

		FVector ActorForward = this->GetActorForwardVector();

		FVector MoveDelta = (this->MoveTarget - CurrentLocation).GetSafeNormal() * Speed * DeltaTime;
		MoveDelta.Z = 0;

		float Angle = acos(MoveDelta.Dot(ActorForward) / (MoveDelta.Length() * ActorForward.Length()));

		float AngleDeg = FMath::RadiansToDegrees(atan2(
			(ActorForward.X * MoveDelta.X) + (ActorForward.Y * MoveDelta.Y),
			(ActorForward.X * MoveDelta.Y) + (ActorForward.Y * MoveDelta.X)
		)) + 180.0f;

		if (this->Crouching || this->Slow) {
			MoveDelta *= 0.5;
		}

		if (abs(AngleDeg - 45.0f) < 90.0f && AngleDeg > 45.0f) {
			LegsState = EUnitAnimLegsState::WalkBwd;
			MoveDelta *= StrafeModifier;
			LegIK.StepDistanceCoef *= 0.5f;
		}
		else if (abs(AngleDeg - (45.0f * 3.0f)) < 90.0f) {
			LegsState = EUnitAnimLegsState::WalkLeft;
			MoveDelta *= StrafeModifier;
			LegIK.StepDistanceCoef *= 0.5f;
		}
		else if (abs(AngleDeg - (45.0f * 5.0f)) < 90.0f) {
			LegsState = EUnitAnimLegsState::WalkFwd;

			if (this->Exerted && !this->Crouching) {
				MoveDelta *= this->SprintModifier;
				LegsModifier = EUnitAnimLegsModifier::Sprint;
				LegIK.StepDistanceCoef *= 1.5f;
			}
		}
		else {
			LegsState = EUnitAnimLegsState::WalkRight;
			MoveDelta *= StrafeModifier;
			LegIK.StepDistanceCoef *= 0.5f;
		}

		LegIK.BodyVelocity = MoveDelta / DeltaTime;
		this->SetActorLocation(CurrentLocation + MoveDelta);
	}
	this->RigComponent->LegIKSetDynamics(LegIK);

	// Jump animation has highest priority.
	if (ActiveJump) LegsState = EUnitAnimLegsState::Jump;

	this->Animation->Script_LegsState = LegsState;
	this->Animation->Script_LegsModifier = LegsModifier;

	// Facing update.
	if (this->HasFaceTarget) {
		FRotator CurrentRotation = this->GetActorRotation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, this->FaceTarget);

		float Speed = this->TurnSpeed;
		if (LowPower) Speed *= 0.2f;

		FRotator NewRotation = FRotator(FQuat::Slerp(CurrentRotation.Quaternion(), LookAtRotation.Quaternion(), DeltaTime * Speed));
		FRotator LockedNewRotation = FRotator(0.0f, NewRotation.Yaw, 0.0f);

		this->SetActorRotation(LockedNewRotation);
	}

	this->Animation->Script_TimeDilation = this->AnimationScale;
}

// IItemHolder implementation.
FVector AUnitPawn::ItemHolderGetLocation() { return this->GetActorLocation(); }

void AUnitPawn::ItemHolderPlaySound(USoundBase* Sound) {
	if (Sound == nullptr) return;

	this->AudioComponent->Sound = Sound;
	this->AudioComponent->Play(0.0f);
}

FRotator AUnitPawn::ItemHolderGetRotation() {
	FRotator Rotation = this->GetActorRotation();
	Rotation.Pitch = this->TorsoPitch;

	return Rotation;
}

float AUnitPawn::ItemHolderGetSpreadModifier() {
	return this->Crouching ? 0.5f : 1.0f;
}

// Callbacks.
void AUnitPawn::ThenFinishInteract() {
	this->CurrentInteractActor->InteractiveUse(this);
	this->CurrentInteractActor->InteractTimer = -1.0f;
	this->CurrentInteractActor = nullptr;
}

void AUnitPawn::ThenFinishUse() {
	this->CurrentUseItem->ItemUse(this->CurrentUseItemTarget);
	this->CurrentUseItem = nullptr;
	this->CurrentUseItemTarget = nullptr;
	this->ArmsActionMoveLock = false;
}

void AUnitPawn::ThenStartReload() {
	AWeaponItem* Weapon = this->UnitGetActiveWeapon();
	
	Weapon->WeaponSwapMagazines(nullptr);

	// Might be scheded for destroy.
	if (!IsValid(this->UnloadingMagazine)) this->UnloadingMagazine = nullptr;

	if (this->UnloadingMagazine != nullptr) {
		this->UnloadingMagazine->AttachToComponent(
			this->ActiveItemAltHostComponent,
			FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				true
			),
			FName("None")
		);

		// Might be scheded for destroy.
		if (!IsValid(this->UnloadingMagazine)) this->UnloadingMagazine = nullptr;

		this->UnloadingMagazine->SetActorRelativeRotation(this->UnloadingMagazine->EquippedRotation);
		this->UnloadingMagazine->SetActorRelativeLocation(this->UnloadingMagazine->EquippedOffset);
		this->UnloadingMagazine->ItemTake(this);
	}
		
	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= Weapon->ReloadMagazineAttachTime * 0.25f;
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenEarlyReload);
}

void AUnitPawn::ThenEarlyReload() {
	AWeaponItem* Weapon = this->UnitGetActiveWeapon();

	if (this->UnloadingMagazine != nullptr) {
		UUnitSlotComponent* AvailSlot = this->UnitGetEmptySlotAllowing(this->UnloadingMagazine->InventoryType);
		if (AvailSlot != nullptr) {
			this->UnloadingMagazine->ItemTake(this);
			AvailSlot->SlotSetContent(this->UnloadingMagazine);
		}
		else {
			this->UnloadingMagazine->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
			this->UnloadingMagazine->ItemDrop(this);
		}
		this->UnloadingMagazine = nullptr;
	}

	this->LoadingMagazine->AttachToComponent(
		this->ActiveItemAltHostComponent,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);
	this->LoadingMagazine->SetActorRelativeRotation(this->LoadingMagazine->EquippedRotation);
	this->LoadingMagazine->SetActorRelativeLocation(this->LoadingMagazine->EquippedOffset);
	
	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= Weapon->ReloadMagazineAttachTime * 0.5f;
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenMidReload);
}

void AUnitPawn::ThenMidReload() {
	AWeaponItem* Weapon = this->UnitGetActiveWeapon();

	Weapon->WeaponSwapMagazines(this->LoadingMagazine);
	this->LoadingMagazine = nullptr;

	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= (1.0f - Weapon->ReloadMagazineAttachTime);
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenPostReload);
}

void AUnitPawn::ThenPostReload() {
	this->ArmsActionMoveLock = false;
}

// Getters.
bool AUnitPawn::UnitIsCrouched() { return this->Crouching; }
bool AUnitPawn::UnitIsExerted() { return this->Exerted; }
AItemActor* AUnitPawn::UnitGetActiveItem() { return this->ActiveItem; }
AArmorItem* AUnitPawn::UnitGetArmor() { return this->ArmorItem; }

int AUnitPawn::UnitGetConcealmentScore() {
	int Best = 0;
	if (this->ActiveItem != nullptr) Best = this->ActiveItem->EquippedConcealment;

	for (int i = 0; i < this->ActiveConcealments.Num(); i++) {
		int Check = (
			this->UnitIsCrouched() ?
				this->ActiveConcealments[i].ScoreCrouched
				:
				this->ActiveConcealments[i].Score
		);

		if (Check > Best) Best = Check;
	}

	return Best;
}

void AUnitPawn::UnitAddConcealment(AActor* Source, int Score, int ScoreCrouched) {
	FUnitConcealment Entry;
	Entry.Source = Source;
	Entry.Score = Score;
	Entry.ScoreCrouched = ScoreCrouched;

	this->ActiveConcealments.Push(Entry);
}

void AUnitPawn::UnitRemoveConcealment(AActor* Source) {
	int RemoveIndex = -1;
	for (int i = 0; i < this->ActiveConcealments.Num(); i++) {
		FUnitConcealment* Check = &this->ActiveConcealments[i];

		if (Check->Source == Source) {
			RemoveIndex = i;
			break;
		}
	}

	if (RemoveIndex >= 0) this->ActiveConcealments.RemoveAt(RemoveIndex);
}

AWeaponItem* AUnitPawn::UnitGetActiveWeapon() {
	if (this->ActiveItem == nullptr) return nullptr;

	return Cast<AWeaponItem>(this->ActiveItem);
}

bool AUnitPawn::UnitAreArmsOccupied() {
	return !this->OverrideArmsState && (
		this->ArmsAnimationTimer > 0.0f ||
		this->ArmsAnimationCooldownTimer > 0.0f ||
		this->CheckingStatus ||
		this->Immobilized
	);
}

bool AUnitPawn::UnitIsJumping() {
	return this->JumpTimer > 0.0f;
}

bool AUnitPawn::UnitIsMoving() {
	return this->HasMoveTarget;
}

bool AUnitPawn::UnitHasFaceTarget() {
	return this->HasFaceTarget;
}

// Inventory.
void AUnitPawn::UnitRawSetActiveItem(AItemActor* Item) {
	this->ActiveItem = Item;

	if (this->ActiveItem == nullptr) return;

	this->ActiveItem->AttachToComponent(
		this->ActiveItem->EquipAltHand ? this->ActiveItemAltHostComponent : this->ActiveItemHostComponent,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);

	this->ActiveItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->ActiveItem->SetActorRelativeLocation(this->ActiveItem->EquippedOffset);
	this->ActiveItem->SetActorRelativeRotation(this->ActiveItem->EquippedRotation);
}

void AUnitPawn::UnitTakeItem(AItemActor* TargetItem) {
	if (TargetItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	// If armor; equip.
	AArmorItem* AsArmor = Cast<AArmorItem>(TargetItem);
	if (AsArmor != nullptr) {
		this->UnitPlayInteractAnimation();

		if (this->ArmorItem != nullptr) {
			this->ArmorItem->ItemDrop(this);
		}

		this->ArmorItem = AsArmor;
		this->ArmorItem->ItemTake(this);
		this->ArmorItem->AttachToComponent(
			this->RigComponent,
			FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				true
			),
			FName("S_Armor")
		);
		this->ArmorItem->SetActorRelativeLocation(this->ArmorItem->EquippedOffset);
		this->ArmorItem->SetActorRelativeRotation(this->ArmorItem->EquippedRotation);
		this->UnitDiscoverDynamicChildComponents();
		return;
	}

	// If equippable and active item has available slot; move current and equip.
	if (TargetItem->Equippable) {
		bool ActiveCleared = this->ActiveItem == nullptr;
		
		if (!ActiveCleared) {
			UUnitSlotComponent* PreviousActiveAvailSlot = this->UnitGetEmptySlotAllowing(this->ActiveItem->InventoryType);
			if (PreviousActiveAvailSlot != nullptr) {
				PreviousActiveAvailSlot->SlotSetContent(this->ActiveItem);
				this->UnitDiscoverDynamicChildComponents();
				this->UnitRawSetActiveItem(nullptr);
				ActiveCleared = true;
			}
		}

		if (ActiveCleared) {
			this->UnitPlayInteractAnimation();

			TargetItem->ItemTake(this);
			this->UnitRawSetActiveItem(TargetItem);
			this->UnitDiscoverDynamicChildComponents();
			return;
		}
	}

	// If empty slot exists; put there.
	UUnitSlotComponent* AvailSlot = this->UnitGetEmptySlotAllowing(TargetItem->InventoryType);
	if (AvailSlot != nullptr) {
		this->UnitPlayInteractAnimation();

		TargetItem->ItemTake(this);
		AvailSlot->SlotSetContent(TargetItem);
		this->UnitDiscoverDynamicChildComponents();
		return;
	}

	// If equippable and no slot for current active; equip and drop current.
	if (TargetItem->Equippable) {
		this->UnitPlayInteractAnimation();

		this->ActiveItem->ItemDrop(this); // Active item known non-null.

		TargetItem->ItemTake(this);
		this->UnitRawSetActiveItem(TargetItem);
		this->UnitDiscoverDynamicChildComponents();
		return;
	}

	// If non-empty slot exists; put there.
	AvailSlot = this->UnitGetSlotAllowing(TargetItem->InventoryType);
	if (AvailSlot != nullptr) {
		this->UnitPlayInteractAnimation();

		AvailSlot->SlotGetContent()->ItemDrop(this); // Known non-null.

		TargetItem->ItemTake(this);
		AvailSlot->SlotSetContent(TargetItem);

		this->UnitDiscoverDynamicChildComponents();
		return;
	}

	// TODO: ??? (Can't take).
	ERR_LOG(this, "take item fail:");
}

void AUnitPawn::UnitDropActiveItem() {
	if (this->ActiveItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayInteractAnimation();

	this->ActiveItem->ItemDrop(this);
	this->UnitRawSetActiveItem(nullptr);
	this->UnitDiscoverDynamicChildComponents();
}

void AUnitPawn::UnitDropItem(AItemActor* Target) {
	if (this->UnitAreArmsOccupied()) return;
	
	if (Target == this->ActiveItem) {
		this->UnitDropActiveItem();
		return;
	}

	if (Target == this->ArmorItem) {
		this->UnitDropArmor();
		return;
	}
	
	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* Content = Check->SlotGetContent();

		if (Content == Target) {
			this->UnitPlayInteractAnimation();
			Content->ItemDrop(this);
			Check->SlotSetContent(nullptr);

			this->UnitDiscoverDynamicChildComponents();
			return;
		};
	}
}

bool AUnitPawn::UnitHasItem(AItemActor* Target) {	
	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		if (Check->SlotGetContent() == Target) return true;
	}

	return false;
}

AItemActor* AUnitPawn::UnitGetItemByName(FString ItemName) {	
	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* CheckItem = Check->SlotGetContent();
		if (CheckItem != nullptr && CheckItem->ItemName == ItemName) return CheckItem;
	}

	return nullptr;
}

AItemActor* AUnitPawn::UnitGetItemByClass(TSubclassOf<AItemActor> ItemClass) {	
	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* CheckItem = Check->SlotGetContent();
		if (CheckItem != nullptr && CheckItem->IsA(ItemClass)) return CheckItem;
	}

	return nullptr;
}

void AUnitPawn::UnitDropArmor() {
	if (this->ArmorItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayInteractAnimation();

	this->ArmorItem->ItemDrop(this);
	this->ArmorItem = nullptr;

	this->UnitDiscoverDynamicChildComponents();
}

void AUnitPawn::UnitDequipActiveItem() {
	if (this->ActiveItem == nullptr) return;
	
	UUnitSlotComponent* AvailSlot = this->UnitGetEmptySlotAllowing(this->ActiveItem->InventoryType);

	AItemActor* PreviousActive = this->ActiveItem;
	this->UnitRawSetActiveItem(nullptr);

	if (AvailSlot != nullptr) {
		AvailSlot->SlotSetContent(PreviousActive);
	}
	else {
		PreviousActive->ItemDrop(this);
		this->UnitDiscoverDynamicChildComponents();
	}
}

void AUnitPawn::UnitEquipItem(AItemActor* Target) {
	if (Target == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* Content = Check->SlotGetContent();

		if (Content == Target && Content->Equippable) {
			this->UnitSetTriggerPulled(false);
			this->UnitPlayInteractAnimation();

			UUnitSlotComponent* TargetSlot = this->Slots[i];

			AItemActor* TargetItem = TargetSlot->SlotGetContent();
			TargetSlot->SlotSetContent(nullptr);

			this->UnitDequipActiveItem();

			this->UnitRawSetActiveItem(TargetItem);
			return;
		};
	}
}

void AUnitPawn::UnitEquipFromSlot(int Index) {
	if (this->UnitAreArmsOccupied()) return;

	TArray<UUnitSlotComponent*> EquippableSlots = this->UnitGetEquippableSlots();
	if (Index >= EquippableSlots.Num()) return;

	this->UnitEquipItem(EquippableSlots[Index]->SlotGetContent());
}

TArray<UUnitSlotComponent*> AUnitPawn::UnitGetEquippableSlots() {
	TArray<UUnitSlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* Content = Check->SlotGetContent();

		if (Content != nullptr && Content->Equippable) Found.Push(Check);
	}

	Found.Sort([](const UUnitSlotComponent& A, const UUnitSlotComponent& B) {
		return A.SlotGetContent()->InventoryType < B.SlotGetContent()->InventoryType;
	});

	return Found;
}

UUnitSlotComponent* AUnitPawn::UnitGetEmptySlotAllowing(EItemInventoryType Type) {
	UUnitSlotComponent* Best = nullptr;
	int BestScore;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		if (!Check->AllowedItems.Contains(Type) || Check->SlotGetContent() != nullptr) continue;

		int AllowedCount = Check->AllowedItems.Num();
		if (Best == nullptr || AllowedCount < BestScore) {
			Best = Check;
			BestScore = AllowedCount;
		}
	}

	return Best;
}

UUnitSlotComponent* AUnitPawn::UnitGetSlotAllowing(EItemInventoryType Type) {
	UUnitSlotComponent* Best = nullptr;
	int BestScore;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		if (!Check->AllowedItems.Contains(Type)) continue;

		int AllowedCount = Check->AllowedItems.Num();
		if (Best == nullptr || AllowedCount < BestScore) {
			Best = Check;
			BestScore = AllowedCount;
		}
	}

	return Best;
}

// TODO: Make template.
TArray<UUnitSlotComponent*> AUnitPawn::UnitGetSlotsContaining(EItemInventoryType Type) {
	TArray<UUnitSlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* Content = Check->SlotGetContent();
		if (Content != nullptr && Content->InventoryType == Type) Found.Push(Check);
	}

	return Found;
}

TArray<UUnitSlotComponent*> AUnitPawn::UnitGetSlotsContainingMagazines(int AmmoTypeID) {
	TArray<UUnitSlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		AItemActor* Content = Check->SlotGetContent();
		if (Content == nullptr) continue;

		AMagazineItem* AsMagazine = Cast<AMagazineItem>(Content);
		if (AsMagazine == nullptr) continue;

		if (AsMagazine->AmmoTypeID == AmmoTypeID) Found.Push(Check);
	}

	return Found;
}

// Animations.
void AUnitPawn::UnitPlayAnimationOnce(EUnitAnimArmsModifier AnimationMod, FAnimationConfig Config, void (AUnitPawn::*Then)()) {
	if (this->OverrideArmsState) return;

	this->ArmsAnimation = AnimationMod;
	this->ArmsAnimationTimer = Config.Time;
	this->ArmsAnimationThen = Then;
}

void AUnitPawn::UnitPlayInteractAnimation() {
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Interact, this->InteractAnimation, nullptr);
}

// Actions.
void AUnitPawn::UnitInteractWith(AActor* Target) {
	float Distance = (Target->GetActorLocation() - this->GetActorLocation()).Size();
	if (Distance > this->UseReach) return;
	
	AInteractiveActor* AsInteractive = Cast<AInteractiveActor>(Target);
	if (AsInteractive == nullptr) return;

	if (this->UnitAreArmsOccupied()) return;

	if (AsInteractive->InteractTime > 0.0f) {
		this->CurrentInteractActor = AsInteractive;
		this->CurrentInteractActor->InteractTimer = 0.0f;

		this->UnitDequipActiveItem();

		FAnimationConfig Config;
		Config.Time = this->CurrentInteractActor->InteractTime;
		this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Interact, Config, &AUnitPawn::ThenFinishInteract);
	}
	else {
		this->UnitPlayInteractAnimation();
		AsInteractive->InteractiveUse(this);
	}
}

void AUnitPawn::UnitSetCheckingStatus(bool NewChecking) {
	if (NewChecking) {
		if (this->UnitAreArmsOccupied()) return;

		this->UnitSetTriggerPulled(false);
	}

	this->CheckingStatus = NewChecking;
}

void AUnitPawn::UnitUseActiveItem(AActor* Target) {
	if (this->UnitAreArmsOccupied()) return;

	if (this->ActiveItem != nullptr && !this->ActiveItem->Usable) return;

	bool InvalidTarget = (
		this->ActiveItem->RequiresTarget && (
			Target == nullptr ||
			!Target->IsA(this->ActiveItem->TargetType) ||
			(Target->GetActorLocation() - this->GetActorLocation()).Size() > this->UseReach
		)
	);
	if (InvalidTarget) return;

	this->CurrentUseItem = this->ActiveItem;
	this->CurrentUseItem->ItemStartUse();
	this->CurrentUseItemTarget = Target;
	if (this->CurrentUseItem->ImmobilizeOnUse) this->ArmsActionMoveLock = true;
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Use, this->ActiveItem->UseAnimation, &AUnitPawn::ThenFinishUse);
}

void AUnitPawn::UnitImmobilize(bool Which) {
	this->Immobilized = Which;
	if (this->Immobilized) {
		this->UnitSetTriggerPulled(false);
	}
}

void AUnitPawn::UnitSetSlow(bool Which) {
	this->Slow = Which;
}

void AUnitPawn::UnitMoveTowards(FVector Target) {
	if (this->JumpTimer > 0.0f) return; // No air control.

	this->MoveTarget = Target;
	this->HasMoveTarget = true;
}

void AUnitPawn::UnitFaceTowards(FVector Target) {
	this->FaceTarget = Target;
	this->HasFaceTarget = true;
}

void AUnitPawn::UnitUpdateTorsoPitch(float TargetValue) {
	this->TargetTorsoPitch = TargetValue;
}

void AUnitPawn::UnitSetCrouched(bool NewCrouch) {
	if (this->JumpTimer > 0.0f) NewCrouch = false;

	this->Crouching = NewCrouch;
}

void AUnitPawn::UnitSetExerted(bool NewExerted) {
	this->Exerted = NewExerted;
}

void AUnitPawn::UnitJump() {
	if (this->JumpTimer > 0.0f || this->Crouching || this->Immobilized) return;

	if (!this->UnitDrainStamina(5.0f)) return;

	// TODO: Kinda busted in slo-mo.
	this->ColliderComponent->AddImpulse(FVector(0.0f, 0.0f, this->JumpStrength), FName("None"), true);

	this->JumpTimer = this->JumpTime;
	this->MoveTarget = this->GetActorForwardVector() * 10000.0f;
}

void AUnitPawn::UnitReload() {
	if (this->UnitAreArmsOccupied()) return;

	AWeaponItem* Weapon = this->UnitGetActiveWeapon();
	if (Weapon == nullptr) return;

	// Find compatible magazine with most ammo.
	TArray<UUnitSlotComponent*> MagazineSlots = this->UnitGetSlotsContainingMagazines(Weapon->AmmoTypeID);
	if (MagazineSlots.Num() == 0) return;

	UUnitSlotComponent* SelectedSlot = MagazineSlots[0];
	AMagazineItem* SelectedMag = Cast<AMagazineItem>(MagazineSlots[0]->SlotGetContent());
	for (int i = 1; i < MagazineSlots.Num(); i++) {
		UUnitSlotComponent* CheckSlot = MagazineSlots[i];
		AMagazineItem* CheckMag = Cast<AMagazineItem>(CheckSlot->SlotGetContent());

		if (CheckMag->Ammo > SelectedMag->Ammo) {
			SelectedSlot = CheckSlot;
			SelectedMag = CheckMag;
		}
	}

	// Swap.
	this->UnitSetTriggerPulled(false);

	SelectedSlot->SlotSetContent(nullptr);

	if (!this->OverrideArmsState) {
		this->ArmsActionMoveLock = Weapon->ImmobilizeOnReload;
		this->LoadingMagazine = SelectedMag;
	}
	this->UnloadingMagazine = Weapon->WeaponGetMagazine();

	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= Weapon->ReloadMagazineAttachTime * 0.25f; // TODO: New var.
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenStartReload);
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->UnitAreArmsOccupied()) return;

	AWeaponItem* Weapon = this->UnitGetActiveWeapon();
	if (Weapon == nullptr) return;

	Weapon->WeaponSetTriggerPulled(NewTriggerPulled);
}

bool AUnitPawn::UnitDrainStamina(float Amount) {
	if (this->Stamina > Amount) {
		this->Stamina -= Amount;
		this->StaminaRegenTimer = 1.0f;
		return true;
	}

	return false;
}

bool AUnitPawn::UnitDrainEnergy(float Amount) {
	if (this->Energy > Amount) {
		this->Energy -= Amount;
		return true;
	}

	return false;
}

void AUnitPawn::UnitHealDamage(FDamageProfile Healing) {
	this->KineticHealth = FMath::Min(this->MaxKineticHealth, this->KineticHealth + Healing.Kinetic);
	this->Energy = FMath::Min(this->MaxEnergy, this->Energy + Healing.Energy);
}

void AUnitPawn::DamagableTakeDamage(FDamageProfile Profile, AActor* Source) {
	float Kinetic = Profile.Kinetic;

	//	Absorb damage with armor.
	if (this->ArmorItem != nullptr) {
		if (Kinetic >= this->ArmorItem->KineticHealth) {
			Kinetic -= this->ArmorItem->KineticHealth;

			AArmorItem* Armor = this->ArmorItem;
			this->ArmorItem = nullptr;
			Armor->Destroy();
		}
		else {
			this->ArmorItem->KineticHealth -= Kinetic;
			Kinetic = 0;
		}
	}

	this->KineticHealth -= Kinetic;
	
	float EnergyDamage = Profile.Energy;
	if (EnergyDamage > this->Energy) {
		EnergyDamage -= this->Energy;
		this->Energy = 0.0f;
		this->Stamina -= EnergyDamage;
		if (this->Stamina < 0.0f) this->Stamina = 0.0f;
	}
	else {
		this->Energy -= EnergyDamage;
	}

	if (this->KineticHealth <= 0.0f) {
		this->UnitDie();
	}
}

void AUnitPawn::UnitDie() {
	this->OverrideArmsState = true;
	
	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Slot = this->Slots[i];

		AItemActor* Content = Slot->SlotGetContent();
		if (Content == nullptr) continue;

		Slot->SlotSetContent(nullptr);
		Content->ItemDrop(this);
	}

	this->UnitDropActiveItem();

	this->GetWorld()->SpawnActor<AActor>(
		this->RagdollType,
		this->GetActorLocation(),
		this->GetActorRotation(),
		FActorSpawnParameters()
	);

	this->Destroy();
}
