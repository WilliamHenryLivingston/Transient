#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "TransientDebug.h"
#include "EquippedMeshConfig.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();

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
		if (Name.Equals("WeaponOffset")) this->WeaponOffsetComponent = Check;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents(StaticMeshComponents, true);
	for (int i = 0; i < StaticMeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = StaticMeshComponents[i];

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
	this->HasStaminaDrain = false;
	this->HasFaceTarget = false;

	if (this->JumpTimer < 0.0f) {
		this->HasMoveTarget = false;
	}
}

void AUnitPawn::UnitPostTick(float DeltaTime) {
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
	if (!this->HasStaminaDrain) {
		this->Stamina = FMath::Min(this->MaxStamina, this->Stamina + (this->StaminaRegen * DeltaTime));
	}

	// Timers and animation updates.
	// ...Arms mode.
	if (this->ActiveItem == nullptr) {
		this->UnitUpdateHostMesh(this->ActiveItemHostComponent, nullptr);
		this->UnitUpdateHostMesh(this->ActiveItemAltHostComponent, nullptr);

		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}
	else {
		if (this->ActiveItem->EquipAltHand) {
			this->UnitUpdateHostMesh(this->ActiveItemHostComponent, nullptr);
			this->UnitUpdateHostMesh(this->ActiveItemAltHostComponent, &this->ActiveItem->EquippedMesh);
		}
		else {
			this->UnitUpdateHostMesh(this->ActiveItemAltHostComponent, nullptr);
			this->UnitUpdateHostMesh(this->ActiveItemHostComponent, &this->ActiveItem->EquippedMesh);
		}

		if (this->Immobilized) {
			this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
		}
		this->Animation->Script_ArmsMode = this->ActiveItem->EquippedAnimArmsMode;
	}
	if (this->ForceArmsEmptyAnimation) {
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}

	// ...Arms modifier (animation).
	if (this->ArmsAnimationTimer > 0.0f) {
		this->ArmsAnimationTimer -= DeltaTime;
		this->Animation->Script_ArmsModifier = this->ArmsAnimation;

		if (this->ArmsAnimationTimer < 0.0f && this->ArmsAnimationThen != nullptr) {
			(this->*(this->ArmsAnimationThen))();
			this->ArmsAnimationThen = nullptr;
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

	if (this->HasMoveTarget && !this->Immobilized) {
		FVector ActorForward = this->GetActorForwardVector();

		FVector MoveDelta = (this->MoveTarget - CurrentLocation).GetSafeNormal() * this->MoveSpeed * DeltaTime;
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

		FRotator NewRotation = FRotator(FQuat::Slerp(CurrentRotation.Quaternion(), LookAtRotation.Quaternion(), DeltaTime * this->TurnSpeed));
		FRotator LockedNewRotation = FRotator(0.0f, NewRotation.Yaw, 0.0f);

		this->SetActorRotation(LockedNewRotation);
	}
}

// IItemHolder implementation.
FVector AUnitPawn::ItemHolderGetLocation() { return this->GetActorLocation(); }

void AUnitPawn::ItemHolderPlaySound(USoundBase* Sound) {
	if (Sound == nullptr) return;

	this->AudioComponent->Sound = Sound;
	this->AudioComponent->Play(0.0f);
}

FVector AUnitPawn::ItemHolderGetWeaponOffset() {
	FVector AdjustedOffset = this->WeaponOffsetComponent->GetRelativeLocation();
	if (this->Crouching) AdjustedOffset.Z *= this->CrouchVerticalShrink;

	return AdjustedOffset;
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
void AUnitPawn::UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config) {
	if (Host == nullptr) return;

	if (Config == nullptr) {
		Host->SetStaticMesh(nullptr);
		return;
	}

	Host->SetStaticMesh(Config->Mesh);
	Host->SetRelativeScale3D(Config->Scale);
	Host->SetRelativeRotation(Config->Rotation);
}

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
void AUnitPawn::UnitTakeItem(AItemActor* TargetItem) {
	if (TargetItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	if (TargetItem->Equippable) {
		this->UnitPlayInteractAnimation();

		this->UnitDequipActiveItem();

		TargetItem->ItemTake(this);
		this->ActiveItem = TargetItem;
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
	if (!TargetItem->UsesEquipMesh) {
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
		TargetItem->SetActorRelativeLocation(TargetItem->DirectAttachOffset);
	}
	PlaceableSlots[0]->SlotSetContent(TargetItem);

	this->UnitDiscoverDynamicChildComponents();
}

void AUnitPawn::UnitDropActiveItem() {
	if (this->ActiveItem == nullptr) return;
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayInteractAnimation();

	this->ActiveItem->ItemDrop(this);
	this->ActiveItem = nullptr;
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

	if (PlaceableSlots.Num() > 0) {
		PlaceableSlots[0]->SlotSetContent(this->ActiveItem);
	}
	else {
		this->ActiveItem->ItemDrop(this);
	}

	this->ActiveItem = nullptr;
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

			this->ActiveItem = TargetItem;
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
	Weapon->WeaponSwapMagazines(SelectedMag->Ammo);
	SelectedMag->Destroy();

	this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Reload, Weapon->ReloadAnimation, nullptr);
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
		this->HasStaminaDrain = true;
		return true;
	}

	return false;
}

void AUnitPawn::UnitTakeDamage(FDamageProfile Profile, AActor* Source) {
	float Kinetic = Profile.KineticDamage;

	//	Absorb damage with armor.
	if (this->ArmorItem != nullptr) {
		if (Kinetic >= this->ArmorItem->Health) {
			Kinetic -= this->ArmorItem->Health;

			AArmorItem* Armor = this->ArmorItem;
			this->ArmorItem = nullptr;
			Armor->Destroy();
		}
		else {
			this->ArmorItem->Health -= Kinetic;
			Kinetic = 0;
		}
	}

	this->Health -= Kinetic;
	if (this->Health <= 0.0f) this->UnitDie();
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
