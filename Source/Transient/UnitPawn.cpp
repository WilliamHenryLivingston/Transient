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

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUnitPawn::BeginPlay() {
	Super::BeginPlay();

	if (this->Weapon != nullptr) {
		this->UnitEquipWeapon(this->Weapon);
	}
}

void AUnitPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AUnitPawn::UnitMoveTowards(FVector Target, float DeltaTime) {
	FVector Move = (Target - this->GetActorLocation()).GetSafeNormal() * this->Speed * DeltaTime;
	FVector ActorForward = this->GetActorForwardVector();

	float Angle = acos(Move.Dot(ActorForward) / (Move.Length() * ActorForward.Length()));
	if (Angle > StrafeConeAngle) {
		Target *= StrafeModifier;
	}

	this->SetActorLocation(this->GetActorLocation() + Move);
}

void AUnitPawn::UnitFaceTowards(FVector Target, float DeltaTime) {
	FRotator CurrentRotation = this->GetActorRotation();
	FVector CurrentLocation = this->GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, Target);

	FRotator NewRotation = FRotator(FQuat::Slerp(CurrentRotation.Quaternion(), LookAtRotation.Quaternion(), DeltaTime * this->TurnSpeed));
	FRotator LockedNewRotation = FRotator(0.0f, NewRotation.Yaw, 0.0f);

	this->OnUnitFace(LockedNewRotation);
	this->SetActorRotation(LockedNewRotation);
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

void AUnitPawn::OnUnitFace(FRotator Rotation) {

}
