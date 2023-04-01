#include "ItemActor.h"

#include "TransientDebug.h"

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

	this->ColliderComponent->SetHiddenInGame(NODEBUG_COLLIDERS);
}

void AItemActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AItemActor::ItemTake(IItemHolder* Target) {
	this->CurrentHolder = Target;

	this->ColliderComponent->SetSimulatePhysics(false);
	this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"), true);
}

void AItemActor::ItemDrop(IItemHolder* Target) {
	this->CurrentHolder = nullptr;

	FRotator ParentRotation = Target->ItemHolderGetRotation();
	FVector DropLocation = Target->ItemHolderGetLocation() + ParentRotation.RotateVector(FVector(150.0f, 0.0f, 0.0f));
	DropLocation.X += FMath::RandRange(-50.0f, 50.0f);
	DropLocation.Y += FMath::RandRange(-50.0f, 50.0f);
	this->SetActorLocation(DropLocation);

	FRotator DropRotation = ParentRotation;
	DropRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	this->SetActorRotation(DropRotation);

	this->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

	this->ColliderComponent->SetCollisionProfileName(FName("Item"), true);
	this->ColliderComponent->SetSimulatePhysics(true);
}

FString AItemActor::ItemGetDescriptorString() {
	return this->ItemName;
}
