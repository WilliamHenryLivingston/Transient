#include "UnitPawn.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AUnitPawn::AUnitPawn() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetSimulatePhysics(true);
	this->ColliderComponent->SetLinearDamping(20.0f);
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
		UnitEquipWeapon(this->Weapon);
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

void AUnitPawn::UnitFaceTowards(FVector Target) {
	FRotator CurrentRotation = this->GetActorRotation();
	FVector CurrentLocation = this->GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, Target);

	FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

	this->OnUnitFace(NewRotation);
	this->SetActorRotation(NewRotation);
}

void AUnitPawn::UnitSetTriggerPulled(bool NewTriggerPulled) {
	if (this->Weapon != nullptr) {
		this->Weapon->WeaponSetTriggerPulled(NewTriggerPulled);
	}
}

void AUnitPawn::UnitTakeDamage(FDamageProfile* Profile) {
	this->Health -= Profile->KineticDamage;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(Health));
}

void AUnitPawn::UnitEquipWeapon(AWeaponActor* NewWeapon) {
	// TODO: Dequip current.

	this->Weapon = NewWeapon;
	this->Weapon->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	this->Weapon->SetActorRelativeLocation(FVector(70.0f, 0.0f, 0.0f));
}

void AUnitPawn::OnUnitFace(FRotator Rotation) {

}
