#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "EquippedMeshConfig.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetSimulatePhysics(true);
	this->ColliderComponent->SetEnableGravity(true);
	this->ColliderComponent->BodyInstance.bLockXRotation = true;
	this->ColliderComponent->BodyInstance.bLockYRotation = true;
	this->ColliderComponent->SetCollisionProfileName(FName("Pawn"), true);
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();
	
	this->UnitDiscoverChildComponents();

	// Initialize inventory.
	this->OverrideArmState = true;

	if (this->WeaponItem != nullptr) this->UnitEquipWeapon(this->WeaponItem);
	if (this->ArmorItem != nullptr) this->UnitEquipArmor(this->ArmorItem);

	TArray<AMagazineItem*> InitMagazines = this->Magazines;
	this->Magazines = TArray<AMagazineItem*>();
	for (int i = 0; i < InitMagazines.Num(); i++) {
		this->UnitEquipMagazine(InitMagazines[i]);
	}

	this->OverrideArmState = false;
}

void AUnitPawn::UnitDiscoverChildComponents() {
	this->RigComponent = Cast<USkeletalMeshComponent>(this->FindComponentByClass(USkeletalMeshComponent::StaticClass()));
	this->Animation = Cast<UUnitAnimInstance>(this->RigComponent->GetAnimInstance());
	this->AudioComponent = Cast<UAudioComponent>(this->FindComponentByClass(UAudioComponent::StaticClass()));

	// Search static meshes for hosts.
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents(StaticMeshComponents, true);
	for (int i = 0; i < StaticMeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = StaticMeshComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("WeaponHost")) this->WeaponHostComponent = Check;
		else if (Name.Equals("ArmorHost")) this->ArmorHostComponent = Check;
		else if (Name.Equals("BackWeaponHost")) this->BackWeaponHostComponent = Check;
		else if (Name.Equals("MagazineReloadHostComponent")) this->MagazineReloadHostComponent = Check;
		else if (Name.Contains("EquipmentMagazine")) this->MagazineHostComponents.Push(Check);
	}
}

void AUnitPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->AudioComponent->PitchMultiplier = this->GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	this->LastHasMoveTarget = this->HasMoveTarget;
	if (this->HasMoveTarget) this->LastMoveTarget = this->MoveTarget;

	// Clear child class targeting for their tick.
	this->TargetTorsoPitch = this->TorsoPitch;
	this->HasStaminaDrain = false;
	this->HasMoveTarget = false;
	this->HasFaceTarget = false;
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
	if (this->ReloadTimer > 0.0f) {
		this->ReloadTimer -= DeltaTime;
		this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::Reload;
	}
	else {
		this->Animation->Script_ArmsModifier = EUnitAnimArmsModifier::None;
	}

	if (this->InteractTimer > 0.0f) this->InteractTimer -= DeltaTime;
	this->Animation->Script_Interacting = this->InteractTimer > 0.0f;

	bool ActiveJump = this->JumpTimer > 0.0f;
	FVector EffectiveMoveTarget = this->MoveTarget;

	FVector CurrentLocation = this->GetActorLocation();

	if (ActiveJump) {
		this->JumpTimer -= DeltaTime;

		this->HasMoveTarget = this->LastHasMoveTarget;
		if (this->HasMoveTarget) {
			// Force move target; no air control. Extend xy-plane move target outwards by an
			// arbitrary amount so it remains valid throughout the jump.
			EffectiveMoveTarget = CurrentLocation + ((this->LastMoveTarget - CurrentLocation) * 1000.0f);
		}
	}
	
	// Movement update.
	EUnitAnimMovementState MovementState = EUnitAnimMovementState::None;

	if (this->Crouching) {
		MovementState = EUnitAnimMovementState::Crouch;
		
		this->HasMoveTarget = false; // Can't move while crouched.
	}

	if (this->HasMoveTarget) {
		FVector ActorForward = this->GetActorForwardVector();

		FVector MoveDelta = (EffectiveMoveTarget - CurrentLocation).GetSafeNormal() * this->MoveSpeed * DeltaTime;
		MoveDelta.Z = 0;

		if (ActiveJump) MoveDelta *= 1.5f; // Move faster in air.

		// Check whether strafing.
		float Angle = acos(MoveDelta.Dot(ActorForward) / (MoveDelta.Length() * ActorForward.Length()));
		if (Angle > this->StrafeConeAngle) MoveDelta *= StrafeModifier;

		float AngleDeg = FMath::RadiansToDegrees(atan2(
			(ActorForward.X * MoveDelta.X) + (ActorForward.Y * MoveDelta.Y),
			(ActorForward.X * MoveDelta.Y) + (ActorForward.Y * MoveDelta.X)
		)) + 180.0f;

		if (abs(AngleDeg - 45.0f) < 90.0f) MovementState = EUnitAnimMovementState::WalkBwd;
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
	FVector AdjustedOffset = this->WeaponOffset;
	if (this->Crouching) AdjustedOffset.Z *= 0.66f;
	
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
void AUnitPawn::UnitPlayGenericInteractionAnimation() {
	if (this->OverrideArmState) return;

	this->InteractTimer = this->InteractAnimationTime;
}

void AUnitPawn::UnitUpdateMagazineHosts() {
	for (int i = 0; i < this->MagazineHostComponents.Num(); i++) {
		FEquippedMeshConfig* NewConfig = nullptr;
		if (i < this->Magazines.Num()) NewConfig = &this->Magazines[i]->EquippedMesh;

		this->UnitUpdateHostMesh(this->MagazineHostComponents[i], NewConfig, true);
	}
}

void AUnitPawn::UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config, bool AltRotation) {
	if (Host == nullptr) return;

	if (Config == nullptr) {
		Host->SetStaticMesh(nullptr);
		return;
	}

	Host->SetStaticMesh(Config->Mesh);
	Host->SetRelativeScale3D(Config->Scale);
	Host->SetRelativeRotation(AltRotation ? Config->AltRotation : Config->Rotation);
}

// Getters.
int AUnitPawn::UnitGetActiveWeaponSlot() { return this->ActiveWeaponSlot; }
int AUnitPawn::UnitGetWeaponSlotCount() { return 2; }
bool AUnitPawn::UnitIsCrouched() { return this->Crouching; }
AWeaponItem* AUnitPawn::UnitGetActiveWeapon() { return this->WeaponItem; }

bool AUnitPawn::UnitAreArmsOccupied() {
	return !this->OverrideArmState && (this->InteractTimer > 0.0f || this->ReloadTimer > 0.0f);
}

bool AUnitPawn::UnitIsJumping() {
	return this->JumpTimer > 0.0f;
}

int AUnitPawn::UnitGetMagazineCountForAmmoType(int TypeID) {
	int Count = 0;

	for (int i = 0; i < this->Magazines.Num(); i++) {
		if (this->Magazines[i]->AmmoTypeID == TypeID) Count++;
	}

	return Count;
}

// Actions.
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
	if (this->JumpTimer > 0.0f || this->Crouching) return;

	// TODO: Kinda busted in slo-mo.
	this->ColliderComponent->AddImpulse(FVector(0.0f, 0.0f, this->JumpStrength), FName("None"), true);

	this->JumpTimer = this->JumpTime;
}

void AUnitPawn::UnitReload() {
	if (this->WeaponItem == nullptr || this->UnitAreArmsOccupied()) return;

	// Find compatible magazine with most ammo.
	int MagazineIndex = -1;
	int BestAmmo = -1;
	for (int i = 0; i < this->Magazines.Num(); i++) {
		AMagazineItem* Check = this->Magazines[i];

		if (Check->AmmoTypeID == this->WeaponItem->AmmoTypeID && Check->Ammo > BestAmmo) {
			MagazineIndex = i;
		}
	}

	if (MagazineIndex == -1) return; // No valid magazine.

	AMagazineItem* Magazine = this->Magazines[MagazineIndex];
	this->Magazines.RemoveAt(MagazineIndex);

	this->UnitSetTriggerPulled(false);
	this->WeaponItem->WeaponSwapMagazines(Magazine->Ammo);
	Magazine->Destroy();

	this->UnitUpdateMagazineHosts();

	this->ReloadTimer = this->WeaponItem->ReloadTime;
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->WeaponItem == nullptr || this->UnitAreArmsOccupied()) return;

	this->WeaponItem->WeaponSetTriggerPulled(NewTriggerPulled);
}

bool AUnitPawn::UnitDrainStamina(float Amount) {
	if (this->Stamina > Amount) {
		this->Stamina -= Amount;
		this->HasStaminaDrain = true;
		return true;
	}

	return false;
}

void AUnitPawn::UnitTakeDamage(FDamageProfile Profile) {
	float Kinetic = Profile.KineticDamage;

	//	Absorb damage with armor.
	if (this->ArmorItem != nullptr) {
		if (Kinetic >= this->ArmorItem->Health) {
			Kinetic -= this->ArmorItem->Health;

			AArmorItem* Armor = this->ArmorItem;
			this->UnitEquipArmor(nullptr);
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
	this->OverrideArmState = true;

	if (this->WeaponItem != nullptr) this->UnitEquipWeapon(nullptr);
	if (this->BackWeaponItem != nullptr) this->BackWeaponItem->ItemDequip(this);
	if (this->ArmorItem != nullptr) this->UnitEquipArmor(nullptr);

	for (int i = 0; i < this->Magazines.Num(); i++) {
		this->Magazines[i]->ItemDequip(this);
	}

	this->Destroy();
}

void AUnitPawn::UnitSwapWeapons() {
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayGenericInteractionAnimation();

	this->UnitSetTriggerPulled(false);

	if (this->ActiveWeaponSlot == 0) this->ActiveWeaponSlot = 1;
	else this->ActiveWeaponSlot = 0;

	AWeaponItem* PreviousActive = this->WeaponItem;
	if (this->BackWeaponItem != nullptr) {
		this->UnitUpdateHostMesh(this->WeaponHostComponent, &this->BackWeaponItem->EquippedMesh, false);
		this->Animation->Script_ArmsMode = this->BackWeaponItem->EquippedAnimArmsMode;
	}
	else {
		this->UnitUpdateHostMesh(this->WeaponHostComponent, nullptr, false);
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}
	this->WeaponItem = this->BackWeaponItem;

	if (PreviousActive != nullptr && !PreviousActive->CanHolster) {
		PreviousActive->ItemDequip(this);
		PreviousActive = nullptr;
	}

	if (PreviousActive != nullptr) {
		this->UnitUpdateHostMesh(this->BackWeaponHostComponent, &PreviousActive->EquippedMesh, true);
	}
	else {
		this->UnitUpdateHostMesh(this->BackWeaponHostComponent, nullptr, true);
	}
	this->BackWeaponItem = PreviousActive;
}

void AUnitPawn::UnitEquipItem(AItemActor* TargetItem) {
	if (TargetItem == nullptr) return;

	AWeaponItem* AsWeapon = Cast<AWeaponItem>(TargetItem);
	if (AsWeapon != nullptr) {
		this->UnitEquipWeapon(AsWeapon);
		return;
	}

	AArmorItem* AsArmor = Cast<AArmorItem>(TargetItem);
	if (AsArmor != nullptr) {
		this->UnitEquipArmor(AsArmor);
		return;
	}

	AMagazineItem* AsMagazine = Cast<AMagazineItem>(TargetItem);
	if (AsMagazine != nullptr) {
		this->UnitEquipMagazine(AsMagazine);
		return;
	}
}

void AUnitPawn::UnitEquipMagazine(AMagazineItem* TargetItem) {
	if (this->UnitAreArmsOccupied()) return;

	this->UnitPlayGenericInteractionAnimation();

	if (this->Magazines.Num() >= this->MagazineHostComponents.Num()) {
		// Out of space, drop one.

		// TODO: Arbitrary = not fun.
		int RemoveIndex = 0;

		this->Magazines[RemoveIndex]->ItemDequip(this);
		this->Magazines.RemoveAt(RemoveIndex);
	}

	TargetItem->ItemEquip(this);

	this->Magazines.Push(TargetItem);
	this->UnitUpdateMagazineHosts();
}

void AUnitPawn::UnitEquipWeapon(AWeaponItem* TargetWeapon) {
	if (this->UnitAreArmsOccupied()) return;

	if (this->WeaponItem == nullptr && TargetWeapon == nullptr) return;

	this->UnitPlayGenericInteractionAnimation();

	if (this->WeaponItem != nullptr && this->WeaponItem != TargetWeapon) {
		this->UnitSetTriggerPulled(false);
		this->WeaponItem->ItemDequip(this);
		this->WeaponItem = nullptr;

		this->UnitUpdateHostMesh(this->WeaponHostComponent, nullptr, false);
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}

	if (TargetWeapon != nullptr) {
		this->WeaponItem = TargetWeapon;

		this->Animation->Script_ArmsMode = this->WeaponItem->EquippedAnimArmsMode;
		this->UnitUpdateHostMesh(this->WeaponHostComponent, &this->WeaponItem->EquippedMesh, false);
		this->WeaponItem->ItemEquip(this);
	}
}

void AUnitPawn::UnitEquipArmor(AArmorItem* TargetArmor) {
	if (this->UnitAreArmsOccupied()) return;
	if (this->ArmorItem == nullptr && TargetArmor == nullptr) return;

	this->UnitPlayGenericInteractionAnimation();

	if (this->ArmorItem != nullptr && this->ArmorItem != TargetArmor) {
		this->ArmorItem->ItemDequip(this);
		this->ArmorItem = nullptr;

		this->UnitUpdateHostMesh(this->ArmorHostComponent, nullptr, false);
	}

	if (TargetArmor != nullptr) {
		this->ArmorItem = TargetArmor;

		this->UnitUpdateHostMesh(this->ArmorHostComponent, &this->ArmorItem->EquippedMesh, false);
		this->ArmorItem->ItemEquip(this);
	}
}
