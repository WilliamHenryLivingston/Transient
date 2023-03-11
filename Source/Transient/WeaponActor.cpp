#include "WeaponActor.h"
#include "ProjectileActor.h"

#include "Components/BoxComponent.h"

AWeaponActor::AWeaponActor() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActor::BeginPlay() {
	Super::BeginPlay();	
}

void AWeaponActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AWeaponActor::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	this->TriggerPulled = NewTriggerPulled;
}
