#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetSimulatePhysics(true);
	this->ColliderComponent->BodyInstance.bLockXRotation = true;
	this->ColliderComponent->BodyInstance.bLockYRotation = true;
	this->ColliderComponent->SetEnableGravity(true);
	this->ColliderComponent->SetCollisionProfileName(FName("Pawn"), true);
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();

	this->RigComponent = Cast<USkeletalMeshComponent>(this->FindComponentByClass(USkeletalMeshComponent::StaticClass()));
	this->Animation = Cast<UnitAnimInstance>(this->RigComponent->GetAnimInstance());

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

	this->HasMoveTarget = false;
	this->HasFaceTarget = false;
}

void AUnitPawn::UnitPostTick(float DeltaTime) {
	EUnitAnimMovementState MovementState = EUnitAnimMovementState::None;

	if (this->ReloadTimer > 0.0f) {
		this->ReloadTimer -= DeltaTime;
	}
	this->Animation->Script_Reloading = this->ReloadTimer > 0.0f;

	if (this->InteractTimer > 0.0f) {
		this->InteractTimer -= DeltaTime;
	}
	this->Animation->Script_Interacting = this->InteractTimer > 0.0f;

	if (this->HasMoveTarget) {
		FVector CurrentLocation = this->GetActorLocation();
		FVector Move = (this->MoveTarget - CurrentLocation).GetSafeNormal() * this->Speed * DeltaTime;
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

FVector AUnitPawn::ItemHolderGetLocation() {
	return this->GetActorLocation();
}

FVector AUnitPawn::ItemHolderGetWeaponOffset() {
	return this->WeaponOffset;
}

FRotator AUnitPawn::ItemHolderGetRotation() {
	return this->GetActorRotation();
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

	this->UnitSetTriggerPulled(false);
	this->WeaponItem->WeaponSwapMagazines(30);
	this->ReloadTimer = this->WeaponItem->ReloadTime;
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->WeaponItem == nullptr || this->UnitArmsOccupied()) return;

	this->WeaponItem->WeaponSetTriggerPulled(NewTriggerPulled);
}

void AUnitPawn::UnitTakeDamage(FDamageProfile Profile) {
	float Kinetic = Profile.KineticDamage;
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

	if (this->Health <= 0.0f) {
		this->UnitDie();
	}
}

void AUnitPawn::UnitDie() {
	this->OverrideArmState = true;
	if (this->WeaponItem != nullptr) {
		this->UnitEquipWeapon(nullptr);
	}
	if (this->ArmorItem != nullptr) {
		this->UnitEquipArmor(nullptr);
	}

	this->Destroy();
}

void AUnitPawn::UnitEquipWeapon(AWeaponItem* NewWeapon) {
	if (this->UnitArmsOccupied() && !this->OverrideArmState) return;

	this->UnitTriggerGenericInteraction();

	if (this->WeaponItem != nullptr && this->WeaponItem != NewWeapon) {
		this->UnitSetTriggerPulled(false);
		this->WeaponItem->ItemDequip(this);
		this->WeaponItem = nullptr;

		this->WeaponHostComponent->SetStaticMesh(nullptr);
		this->Animation->Script_ArmsMode = EUnitAnimArmsMode::Empty;
	}

	if (NewWeapon != nullptr) {
		this->WeaponItem = NewWeapon;

		this->Animation->Script_ArmsMode = this->WeaponItem->EquippedAnimArmsMode;
		this->WeaponHostComponent->SetStaticMesh(this->WeaponItem->EquippedMesh);
		this->WeaponItem->ItemEquip(this);
	}
}

void AUnitPawn::UnitEquipArmor(AArmorItem* NewArmor) {
	if (this->UnitArmsOccupied() && !this->OverrideArmState) return;

	this->UnitTriggerGenericInteraction();

	if (this->ArmorItem != nullptr && this->ArmorItem != NewArmor) {
		this->ArmorItem->ItemDequip(this);
		this->ArmorItem = nullptr;

		this->ArmorHostComponent->SetStaticMesh(nullptr);
	}

	if (NewArmor != nullptr) {
		this->ArmorItem = NewArmor;

		this->ArmorHostComponent->SetStaticMesh(this->ArmorItem->EquippedMesh);
		this->ArmorItem->ItemEquip(this);
	}
}
