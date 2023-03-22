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

	this->RigComponent = Cast<USkeletalMeshComponent>(this->FindComponentByClass(USkeletalMeshComponent::StaticClass()));
	this->Animation = Cast<UnitAnimInstance>(this->RigComponent->GetAnimInstance());

	this->AudioComponent = Cast<UAudioComponent>(this->FindComponentByClass(UAudioComponent::StaticClass()));

	// Discover child mesh hosts.
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents(StaticMeshComponents, true);

	for (int i = 0; i < StaticMeshComponents.Num(); i++) {
		UStaticMeshComponent* Check = StaticMeshComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("WeaponHost")) {
			this->WeaponHostComponent = Check;
		}
		else if (Name.Equals("ArmorHost")) {
			this->ArmorHostComponent = Check;
		}
		else if (Name.Equals("BackWeaponHost")) {
			this->BackWeaponHostComponent = Check;
		}
		else if (Name.Equals("MagazineReloadHostComponent")) {
			this->MagazineReloadHostComponent = Check;
		}
		else if (Name.Contains("EquipmentMagazine")) {
			this->MagazineHostComponents.Push(Check);
		}
	}

	// Equip editor-set items.
	this->OverrideArmState = true;
	if (this->WeaponItem != nullptr) {
		this->UnitEquipWeapon(this->WeaponItem);
	}
	if (this->ArmorItem != nullptr) {
		this->UnitEquipArmor(this->ArmorItem);
	}
	this->OverrideArmState = false;
}

void AUnitPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->AudioComponent->PitchMultiplier = this->GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	this->HasStaminaDrain = false;
	if (this->HasMoveTarget) {
		this->LastMoveTarget = this->MoveTarget;
	}
	else {
		this->LastMoveTarget = this->GetActorLocation();
	}
	this->HasMoveTarget = false;
	this->HasFaceTarget = false;
}

void AUnitPawn::UnitPostTick(float DeltaTime) {
	EUnitAnimMovementState MovementState = EUnitAnimMovementState::None;

	this->Animation->Script_TorsoYRotation = -this->RootPitch;

	if (!this->HasStaminaDrain) {
		this->Stamina += 10.0f * DeltaTime;
		if (this->Stamina > this->MaxStamina) {
			this->Stamina = this->MaxStamina;
		}
	}

	if (this->ReloadTimer > 0.0f) {
		this->ReloadTimer -= DeltaTime;
	}
	this->Animation->Script_Reloading = this->ReloadTimer > 0.0f;

	if (this->InteractTimer > 0.0f) {
		this->InteractTimer -= DeltaTime;
	}
	this->Animation->Script_Interacting = this->InteractTimer > 0.0f;

	if (this->JumpTimer > 0.0f) {
		this->JumpTimer -= DeltaTime;

		// TODO: private this->Crouching instead.
		this->Crouching = false;
		this->HasMoveTarget = true;
		this->MoveTarget = JumpMoveTarget;
	}
	
	if (this->Crouching) {
		this->HasMoveTarget = false;
		MovementState = EUnitAnimMovementState::Crouch;
	}

	if (this->HasMoveTarget) {
		FVector CurrentLocation = this->GetActorLocation();
		FVector Move = (this->MoveTarget - CurrentLocation).GetSafeNormal() * this->Speed * DeltaTime;
		Move.Z = 0;
		if (this->JumpTimer > 0.0f) {
			Move *= 1.5f;
		}
		FVector ActorForward = this->GetActorForwardVector();

		float Angle = acos(Move.Dot(ActorForward) / (Move.Length() * ActorForward.Length()));
		if (Angle > StrafeConeAngle) {
			Move *= StrafeModifier;
		}

		// Cleanup + left / right anims flipped?
		float AngleDeg = FMath::RadiansToDegrees(
			atan2(
				(ActorForward.X * Move.X) + (ActorForward.Y * Move.Y),
				(ActorForward.X * Move.Y) + (ActorForward.Y * Move.X)
			)
		) + 180.0f;
		if (abs(AngleDeg - 45.0f) < 90.0f) {
			MovementState = EUnitAnimMovementState::WalkBwd;
		}
		else if (abs(AngleDeg - (45.0f * 3.0f)) < 90.0f) {
			MovementState = EUnitAnimMovementState::WalkLeft;
		}
		else if (abs(AngleDeg - (45.0f * 5.0f)) < 90.0f) {
			MovementState = EUnitAnimMovementState::WalkFwd;
		}
		else {
			MovementState = EUnitAnimMovementState::WalkRight;
		}
		this->SetActorLocation(CurrentLocation + Move);
	}

	if (this->JumpTimer > 0.0f) {
		MovementState = EUnitAnimMovementState::Jump;
	}

	this->Animation->Script_MovementState = MovementState;

	if (this->HasFaceTarget) {
		FRotator CurrentRotation = this->GetActorRotation();
		FVector CurrentLocation = this->GetActorLocation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, this->FaceTarget);

		FRotator NewRotation = FRotator(FQuat::Slerp(CurrentRotation.Quaternion(), LookAtRotation.Quaternion(), DeltaTime * this->TurnSpeed));
		FRotator LockedNewRotation = FRotator(0.0f, NewRotation.Yaw, 0.0f);

		this->SetActorRotation(LockedNewRotation);
	}
}

void AUnitPawn::ItemHolderPlaySound(USoundBase* Sound) {
	if (Sound == nullptr) return;
	this->AudioComponent->Sound = Sound;
	this->AudioComponent->Play(0.0f);
}

FVector AUnitPawn::ItemHolderGetLocation() {
	return this->GetActorLocation();
}

FVector AUnitPawn::ItemHolderGetWeaponOffset() {
	FVector AdjustedOffset = this->WeaponOffset;
	if (this->Crouching) {
		AdjustedOffset.Z *= 0.66f;
	}
	
	return AdjustedOffset;
}

FRotator AUnitPawn::ItemHolderGetRotation() {
	FRotator Rotation = this->GetActorRotation();
	Rotation.Pitch = this->RootPitch;
	return Rotation;
}

float AUnitPawn::ItemHolderGetSpreadModifier() {
	if (this->Crouching) {
		return 0.5f;
	}
	return 1.0f;
}

void AUnitPawn::UnitSetCrouched(bool NewCrouch) {
	if (this->JumpTimer > 0.0f) {
		NewCrouch = false;
	} 

	this->Crouching = NewCrouch;
}

bool AUnitPawn::UnitIsCrouched() {
	return this->Crouching;
}

void AUnitPawn::UnitJump() {
	if (this->JumpTimer > 0.0f || this->Crouching) return;

	this->ColliderComponent->AddImpulse(FVector(0.0f, 0.0f, this->JumpStrength), FName("None"), true);
	this->JumpTimer = this->JumpTime;
	this->JumpMoveTarget = this->LastMoveTarget;
}

bool AUnitPawn::UnitIsJumping() {
	return this->JumpTimer > 0.0f;
}

void AUnitPawn::UnitTriggerGenericInteraction() {
	this->InteractTimer = this->InteractAnimTime;
}

void AUnitPawn::UnitMoveTowards(FVector Target) {
	this->MoveTarget = Target;
	this->HasMoveTarget = true;
}

void AUnitPawn::UnitFaceTowards(FVector Target) {
	this->FaceTarget = Target;
	this->HasFaceTarget = true;
}

bool AUnitPawn::UnitArmsOccupied() {
	return this->InteractTimer > 0.0f || this->ReloadTimer > 0.0f;
}

void AUnitPawn::UnitReload() {
	if (this->WeaponItem == nullptr || this->UnitArmsOccupied()) return;

	int MagazineIndex = -1;
	for (int i = 0; i < this->Magazines.Num(); i++) {
		// TODO: Smarter.
		if (this->Magazines[i]->AmmoTypeID == this->WeaponItem->AmmoTypeID) {
			MagazineIndex = i;
			break;
		}
	}

	if (MagazineIndex == -1) return;
	AMagazineItem* Magazine = this->Magazines[MagazineIndex];
	this->Magazines.RemoveAt(MagazineIndex);

	this->UnitSetTriggerPulled(false);
	this->WeaponItem->WeaponSwapMagazines(Magazine->Ammo);
	Magazine->ItemDestroy();
	
	this->UnitUpdateMagazineMeshes();

	this->ReloadTimer = this->WeaponItem->ReloadTime;
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->WeaponItem == nullptr || this->UnitArmsOccupied()) return;

	this->WeaponItem->WeaponSetTriggerPulled(NewTriggerPulled);
}

void AUnitPawn::UnitUpdateHostMesh(UStaticMeshComponent* Host, FEquippedMeshConfig* Config, bool AltRotation) {
	if (Host == nullptr) return;
	
	if (Config != nullptr) {
		Host->SetStaticMesh(Config->Mesh);
		Host->SetRelativeScale3D(Config->Scale);
		if (AltRotation) {
			Host->SetRelativeRotation(Config->AltRotation);
		}
		else {
			Host->SetRelativeRotation(Config->Rotation);
		}
	}
	else {
		Host->SetStaticMesh(nullptr);
	}
}

void AUnitPawn::UnitUpdateMagazineMeshes() {
	for (int i = 0; i < this->MagazineHostComponents.Num(); i++) {
		if (i < this->Magazines.Num()) {
			this->UnitUpdateHostMesh(this->MagazineHostComponents[i], &this->Magazines[i]->EquippedMesh, true);
		}
		else {
			this->UnitUpdateHostMesh(this->MagazineHostComponents[i], nullptr, true);
		}
	}
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
	if (this->ArmorItem != nullptr) {
		if (Kinetic >= this->ArmorItem->Health) {
			Kinetic -= this->ArmorItem->Health;

			AArmorItem* Armor = this->ArmorItem;
			this->UnitEquipArmor(nullptr);
			Armor->ItemDestroy();
		}
		else {
			this->ArmorItem->Health -= Kinetic;
			Kinetic = 0;
		}
	}
	this->Health -= Kinetic;

	if (this->Health <= 0.0f) {
		this->UnitDie();
	}
}

void AUnitPawn::UnitDie() {
	this->OverrideArmState = true;
	if (this->WeaponItem != nullptr) {
		this->UnitEquipWeapon(nullptr);
	}
	if (this->BackWeaponItem != nullptr) {
		this->BackWeaponItem->ItemDequip(this);
	}
	if (this->ArmorItem != nullptr) {
		this->UnitEquipArmor(nullptr);
	}
	for (int i = 0; i < this->Magazines.Num(); i++) {
		this->Magazines[i]->ItemDequip(this);
	}

	this->Destroy();
}

void AUnitPawn::UnitSwapWeapons() {
	if (this->UnitArmsOccupied()) return;

	this->UnitTriggerGenericInteraction();

	this->UnitSetTriggerPulled(false);

	if (this->ActiveWeaponSlot == 0) {
		this->ActiveWeaponSlot = 1;
	}
	else {
		this->ActiveWeaponSlot = 0;
	}

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
	if (this->UnitArmsOccupied() && !this->OverrideArmState) return;

	this->UnitTriggerGenericInteraction();

	if (this->Magazines.Num() >= this->MagazineHostComponents.Num()) {
		int RemoveIndex = 0; // TODO: Smart decide.

		this->Magazines[RemoveIndex]->ItemDequip(this);
		this->Magazines.RemoveAt(RemoveIndex);
	}

	TargetItem->ItemEquip(this);
	this->Magazines.Push(TargetItem);

	this->UnitUpdateMagazineMeshes();
}

void AUnitPawn::UnitEquipWeapon(AWeaponItem* TargetWeapon) {
	if (this->UnitArmsOccupied() && !this->OverrideArmState) return;

	if (this->WeaponItem == nullptr && TargetWeapon == nullptr) return;

	this->UnitTriggerGenericInteraction();

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
	if (this->UnitArmsOccupied() && !this->OverrideArmState) return;

	if (this->ArmorItem == nullptr && TargetArmor == nullptr) return;

	this->UnitTriggerGenericInteraction();

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
