#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "TransientDebug.h"
#include "ProjectileWeapon.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();

	this->ReloadingMoveLock = false;

	this->JumpTimer = -1.0f; // TODO: Janky.
	
	this->RigComponent = this->FindComponentByClass<USkeletalMeshComponent>();
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
	}

	this->ColliderComponent->SetHiddenInGame(NODEBUG_COLLIDERS);
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
	this->GetAttachedActors(Attached, false);

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

	// Clear child class targeting for their tick.
	this->TargetTorsoPitch = this->TorsoPitch;
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
	if (this->ActiveItem == nullptr) {
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}
	else {
		this->ActiveItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)); // TODO: What the fuck?

		if (this->Immobilized) {
			this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
		}
		this->Animation->Script_ArmsMode = this->ActiveItem->EquippedAnimArmsMode;
	}
	if (this->ForceArmsEmptyAnimation) {
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
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
		if (CurrentWeapon != nullptr && CurrentWeapon->WeaponGetTriggerPulled()) {
			this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::Fire;
		}
		else {
			this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::None;
		}
	}

	// Movement update.
	FVector CurrentLocation = this->GetActorLocation();	

	bool ActiveJump = this->JumpTimer > 0.0f;
	if (ActiveJump) this->JumpTimer -= DeltaTime;

	EUnitAnimMovementState MovementState = EUnitAnimMovementState::None;

	FVector ColliderScale = this->ColliderComponent->GetRelativeScale3D();
	ColliderScale.Z = this->BaseColliderVerticalScale;
	FVector RigLocation = this->RigComponent->GetRelativeLocation();
	RigLocation.Z = this->BaseRigVerticalOffset;
	FVector RigScale = this->BaseRigScale;
	if (this->Crouching) {
		MovementState = EUnitAnimMovementState::Crouch;
		ColliderScale.Z *= this->CrouchVerticalShrink;
		RigLocation.Z += this->CrouchVerticalTranslate;
		RigScale.Z *= this->BaseColliderVerticalScale / ColliderScale.Z;
		
		this->HasMoveTarget = false;
	}
	this->ColliderComponent->SetRelativeScale3D(ColliderScale);
	this->RigComponent->SetRelativeLocation(RigLocation);
	this->RigComponent->SetRelativeScale3D(RigScale);

	if (this->HasMoveTarget && !this->Immobilized && !this->ReloadingMoveLock) {
		float Speed = this->MoveSpeed;
		if (LowPower) Speed *= 0.25f;

		FVector ActorForward = this->GetActorForwardVector();

		FVector MoveDelta = (this->MoveTarget - CurrentLocation).GetSafeNormal() * Speed * DeltaTime;
		MoveDelta.Z = 0;

		// Check whether strafing.
		float Angle = acos(MoveDelta.Dot(ActorForward) / (MoveDelta.Length() * ActorForward.Length()));
		if (Angle > this->StrafeConeAngle) MoveDelta *= StrafeModifier;

		float AngleDeg = FMath::RadiansToDegrees(atan2(
			(ActorForward.X * MoveDelta.X) + (ActorForward.Y * MoveDelta.Y),
			(ActorForward.X * MoveDelta.Y) + (ActorForward.Y * MoveDelta.X)
		)) + 180.0f;

		if (abs(AngleDeg - 45.0f) < 90.0f && AngleDeg > 45.0f) MovementState = EUnitAnimMovementState::WalkBwd;
		else if (abs(AngleDeg - (45.0f * 3.0f)) < 90.0f) MovementState = EUnitAnimMovementState::WalkLeft;
		else if (abs(AngleDeg - (45.0f * 5.0f)) < 90.0f) MovementState = EUnitAnimMovementState::WalkFwd;
		else MovementState = EUnitAnimMovementState::WalkRight;

		this->SetActorLocation(CurrentLocation + MoveDelta);
	}

	// Jump animation has highest priority.
	if (ActiveJump) MovementState = EUnitAnimMovementState::Jump;

	this->Animation->Script_MovementState = MovementState;

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

// Internals.
void AUnitPawn::UnitFinishUse() {
	this->CurrentUseItem->ItemUse(this->CurrentUseItemTarget);
}

// Getters.
bool AUnitPawn::UnitIsCrouched() { return this->Crouching; }
AItemActor* AUnitPawn::UnitGetActiveItem() { return this->ActiveItem; }
AArmorItem* AUnitPawn::UnitGetArmor() { return this->ArmorItem; }

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

bool AUnitPawn::UnitHasFaceTarget() {
	return this->HasFaceTarget;
}

// Inventory.
void AUnitPawn::UnitRawSetActiveItem(AItemActor* Item) {
	this->ActiveItem = Item;

	if (this->ActiveItem == nullptr) return;

	if (this->ActiveItem->EquipAltHand) {
		this->ActiveItem->AttachToComponent(
			this->ActiveItemAltHostComponent,
			FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				true
			),
			FName("None")
		);
	}
	else {
		this->ActiveItem->AttachToComponent(
			this->ActiveItemHostComponent,
			FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				true
			),
			FName("None")
		);
	}

	this->ActiveItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->ActiveItem->SetActorRelativeLocation(this->ActiveItem->EquippedOffset);
	this->ActiveItem->SetActorRelativeRotation(this->ActiveItem->EquippedRotation);
}

void AUnitPawn::UnitTakeItem(AItemActor* TargetItem) {
	if (TargetItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	if (TargetItem->Equippable) {
		this->UnitPlayInteractAnimation();

		this->UnitDequipActiveItem();

		TargetItem->ItemTake(this);
		this->UnitRawSetActiveItem(TargetItem);
		return;
	}

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
		this->UnitDiscoverDynamicChildComponents();
		return;
	}

	TArray<UUnitSlotComponent*> PlaceableSlots = this->UnitGetEmptySlotsAllowing(TargetItem->InventoryType);
	if (PlaceableSlots.Num() == 0) {
		PlaceableSlots = this->UnitGetSlotsAllowing(TargetItem->InventoryType);
	}

	if (PlaceableSlots.Num() == 0) return;

	this->UnitPlayInteractAnimation();

	AItemActor* PreviousItem = PlaceableSlots[0]->SlotGetContent();
	if (PreviousItem != nullptr) {
		PreviousItem->ItemDrop(this);
		PlaceableSlots[0]->SlotSetContent(nullptr);
	}

	TargetItem->ItemTake(this);
	TargetItem->AttachToComponent(
		this->RigComponent,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("S_Armor") // TODO: Rename slot.
	);
	TargetItem->SetActorRelativeLocation(TargetItem->EquippedOffset);
	PlaceableSlots[0]->SlotSetContent(TargetItem);

	this->UnitDiscoverDynamicChildComponents();
}

void AUnitPawn::UnitDropActiveItem() {
	if (this->ActiveItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayInteractAnimation();

	this->ActiveItem->ItemDrop(this);
	this->UnitRawSetActiveItem(nullptr);
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
	
	TArray<UUnitSlotComponent*> PlaceableSlots = this->UnitGetEmptySlotsAllowing(this->ActiveItem->InventoryType);

	AItemActor* PreviousActive = this->ActiveItem;
	this->UnitRawSetActiveItem(nullptr);

	if (PlaceableSlots.Num() > 0) {
		PlaceableSlots[0]->SlotSetContent(PreviousActive);
	}
	else {
		PreviousActive->ItemDrop(this);
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

	// TODO: Sort.

	return Found;
}

TArray<UUnitSlotComponent*> AUnitPawn::UnitGetEmptySlotsAllowing(EItemInventoryType Type) {
	TArray<UUnitSlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		if (Check->AllowedItems.Contains(Type) && Check->SlotGetContent() == nullptr) Found.Push(Check);
	}

	return Found;
}

TArray<UUnitSlotComponent*> AUnitPawn::UnitGetSlotsAllowing(EItemInventoryType Type) {
	TArray<UUnitSlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UUnitSlotComponent* Check = this->Slots[i];

		if (Check->AllowedItems.Contains(Type)) Found.Push(Check);
	}

	return Found;
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
void AUnitPawn::UnitSetCheckingStatus(bool NewChecking) {
	if (NewChecking) {
		if (this->UnitAreArmsOccupied()) return;

		this->UnitSetTriggerPulled(false);
	}

	this->CheckingStatus = NewChecking;
}

void AUnitPawn::UnitUseActiveItem(AActor* Target) {
	if (this->UnitAreArmsOccupied()) return;

	AUsableItem* AsUsable = Cast<AUsableItem>(this->ActiveItem);
	if (AsUsable == nullptr) return;

	bool InvalidTarget = (
		AsUsable->RequiresTarget && (
			Target == nullptr ||
			!Target->IsA(AsUsable->TargetType) ||
			(Target->GetActorLocation() - this->GetActorLocation()).Size() > this->UseReach
		)
	);
	if (InvalidTarget) return;

	this->CurrentUseItem = AsUsable;
	this->CurrentUseItemTarget = Target;
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Use, AsUsable->UseAnimation, &AUnitPawn::UnitFinishUse);
}

void AUnitPawn::UnitImmobilize(bool Which) {
	this->Immobilized = Which;
	if (this->Immobilized) {
		this->UnitSetTriggerPulled(false);
	}
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

	this->ReloadingMoveLock = Weapon->ImmobilizeOnReload;
	this->LoadingMagazine = SelectedMag;
	
	SelectedMag->AttachToComponent(
		this->ActiveItemAltHostComponent,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);
	SelectedMag->SetActorRelativeRotation(SelectedMag->EquippedRotation);
	SelectedMag->SetActorRelativeLocation(SelectedMag->EquippedOffset);
	Weapon->WeaponSwapMagazines(nullptr);
	
	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= Weapon->ReloadMagazineAttachTime;
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenMidReload);
}

void AUnitPawn::ThenMidReload() {
	// TODO: Safety.
	AWeaponItem* Weapon = this->UnitGetActiveWeapon();
	if (Weapon == nullptr) return;

	Weapon->WeaponSwapMagazines(this->LoadingMagazine);
	this->LoadingMagazine = nullptr;

	FAnimationConfig Config = Weapon->ReloadAnimation;
	Config.Time *= (1.0f - Weapon->ReloadMagazineAttachTime);
	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Config, &AUnitPawn::ThenPostReload);
}

void AUnitPawn::ThenPostReload() {
	this->ReloadingMoveLock = false;
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

void AUnitPawn::UnitTakeDamage(FDamageProfile Profile, AActor* Source) {
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

	this->Destroy();
}
