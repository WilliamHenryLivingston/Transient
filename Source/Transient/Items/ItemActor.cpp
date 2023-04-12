// Copyright: R. Saxifrage, 2023. All rights reserved.

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
	
    if (!IsValid(Cast<AActor>(this->CurrentHolder))) this->CurrentHolder = nullptr;
}

void AItemActor::ItemStartUse() { }

void AItemActor::ItemUse(AActor* Target) { }

void AItemActor::ItemTake(IItemHolder* Target) {
	this->CurrentHolder = Target;

	this->ColliderComponent->SetSimulatePhysics(false);
	this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"), true);
}

void AItemActor::ItemDrop(AActor* Target) {
	this->CurrentHolder = nullptr;

	FRotator ParentRotation = Target->GetActorRotation();
	FVector DropLocation = Target->GetActorLocation() + ParentRotation.RotateVector(FVector(50.0f, 0.0f, 0.0f));
	float DropRadius = 150.0f;
	DropLocation.X += FMath::RandRange(-DropRadius, DropRadius);
	DropLocation.Y += FMath::RandRange(-DropRadius, DropRadius);
	this->SetActorLocation(DropLocation);

	FRotator DropRotation = ParentRotation;
	DropRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	this->SetActorRotation(DropRotation);

	this->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

	FName WorldProfile = FName("Item");
	if (this->WorldCollisionProfile.Len() > 0) {
		WorldProfile = FName(*this->WorldCollisionProfile);
	}
	this->ColliderComponent->SetCollisionProfileName(WorldProfile, true);
	this->ColliderComponent->SetSimulatePhysics(true);
}

FString AItemActor::ItemGetDescriptorString() {
	return this->ItemName;
}
