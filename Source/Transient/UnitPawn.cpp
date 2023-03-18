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

	if (this->Weapon != nullptr) {
		this->UnitEquipWeapon(this->Weapon);
	}

	this->RigComponent = Cast<USkeletalMeshComponent>(this->FindComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (this->RigComponent != nullptr) {
		this->Animation = Cast<UnitAnimInstance>(this->RigComponent->GetAnimInstance());
	}
}

void AUnitPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->HasMoveTarget = false;
	this->HasFaceTarget = false;
}

void AUnitPawn::UnitPostTick(float DeltaTime) {
	if (this->Animation != nullptr) {
		this->Animation->ScriptAuto_IsMoving = this->HasMoveTarget;
	}

	if (this->HasMoveTarget) {
		FVector CurrentLocation = this->GetActorLocation();
		FVector Move = (this->MoveTarget - CurrentLocation).GetSafeNormal() * this->Speed * DeltaTime;
		FVector ActorForward = this->GetActorForwardVector();

		float Angle = acos(Move.Dot(ActorForward) / (Move.Length() * ActorForward.Length()));
		if (Angle > StrafeConeAngle) {
			Move *= StrafeModifier;
		}

		this->SetActorLocation(CurrentLocation + Move);
	}

	if (this->HasFaceTarget) {
		FRotator CurrentRotation = this->GetActorRotation();
		FVector CurrentLocation = this->GetActorLocation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, this->FaceTarget);

		FRotator NewRotation = FRotator(FQuat::Slerp(CurrentRotation.Quaternion(), LookAtRotation.Quaternion(), DeltaTime * this->TurnSpeed));
		FRotator LockedNewRotation = FRotator(0.0f, NewRotation.Yaw, 0.0f);

		this->SetActorRotation(LockedNewRotation);
	}
}

void AUnitPawn::UnitMoveTowards(FVector Target) {
	this->MoveTarget = Target;
	this->HasMoveTarget = true;
}

void AUnitPawn::UnitFaceTowards(FVector Target) {
	this->FaceTarget = Target;
	this->HasFaceTarget = true;
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->Weapon != nullptr) {
		this->Weapon->WeaponSetTriggerPulled(NewTriggerPulled);
	}
}

void AUnitPawn::UnitTakeDamage(FDamageProfile* Profile) {
	this->Health -= Profile->KineticDamage;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(Health));

	if (this->Health <= 0.0f) {
		this->UnitDie();
	}
}

void AUnitPawn::UnitDie() {
	if (this->Weapon != nullptr) {
		this->UnitEquipWeapon(nullptr);
	}

	this->Destroy();
}

void AUnitPawn::UnitEquipWeapon(AWeaponActor* NewWeapon) {
	if (this->Weapon != nullptr && this->Weapon != NewWeapon) {
		this->Weapon->WeaponOnDequip();
		this->Weapon = nullptr;
	}

	if (NewWeapon != nullptr) {
		this->Weapon = NewWeapon;
		this->Weapon->WeaponOnEquip(this);
	}
}

AWeaponActor* AUnitPawn::UnitGetWeapon() {
	return this->Weapon;
}

TArray<AWeaponActor*> AUnitPawn::UnitGetNearbyWeapons() {
	FVector CurrentLocation = this->GetActorLocation();

	TArray<AWeaponActor*> NearbyWeapons;

	for (int i = 0; i < AWeaponActor::WorldItems.Num(); i++) {
		AWeaponActor* CheckWeapon = AWeaponActor::WorldItems[i];

		float Distance = (CheckWeapon->GetActorLocation() - CurrentLocation).Size();

		if (Distance < 100.0f) {
			NearbyWeapons.Push(CheckWeapon);
		}
	}

	return NearbyWeapons;
}
