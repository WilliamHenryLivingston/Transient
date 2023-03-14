#include "WeaponActor.h"

#include "ProjectileActor.h"

#include "Components/BoxComponent.h"

TArray<AWeaponActor*> AWeaponActor::WorldItems = TArray<AWeaponActor*>();

AWeaponActor::AWeaponActor() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActor::BeginPlay() {
	Super::BeginPlay();	

	AWeaponActor::WorldItems.Push(this);
}

void AWeaponActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AWeaponActor::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	this->TriggerPulled = NewTriggerPulled;
}

void AWeaponActor::WeaponOnEquip(AActor* Unit) {
	AWeaponActor::WorldItems.Remove(this);

	this->AttachToActor(Unit, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	this->SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	this->SetActorRelativeLocation(FVector(70.0f, 0.0f, 0.0f));
}

void AWeaponActor::WeaponOnDequip() {
	AWeaponActor::WorldItems.Push(this);

	this->WeaponSetTriggerPulled(false);
	this->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
}
