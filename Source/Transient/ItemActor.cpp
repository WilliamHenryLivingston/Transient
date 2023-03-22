#include "ItemActor.h"

AItemActor::AItemActor() {
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetSimulatePhysics(true);
	this->ColliderComponent->SetEnableGravity(true);
	this->ColliderComponent->SetCollisionProfileName(FName("Item"), true);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::BeginPlay() {
	Super::BeginPlay();
}

void AItemActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AItemActor::ItemEquip(IItemHolder* Target) {
	this->CurrentHolder = Target;

	this->SetActorHiddenInGame(true);
	this->ColliderComponent->SetSimulatePhysics(false);
	this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"), true);
}

void AItemActor::ItemDequip(IItemHolder* Target) {
	this->CurrentHolder = nullptr;

	FVector DropLocation = Target->ItemHolderGetLocation();
	DropLocation.X += FMath::RandRange(-50.0f, 50.0f);
	DropLocation.Y += FMath::RandRange(-50.0f, 50.0f);
	this->SetActorLocation(DropLocation);

	this->SetActorHiddenInGame(false);
	this->ColliderComponent->SetCollisionProfileName(FName("Item"), true);
	this->ColliderComponent->SetSimulatePhysics(true);
}

void AItemActor::ItemDestroy() {
	this->Destroy();
}