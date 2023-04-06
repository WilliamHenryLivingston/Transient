// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "GrassVolumeActor.h"

#include "Kismet/KismetMathLibrary.h"

AGrassVolumeActor::AGrassVolumeActor() {
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->BoundsComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
}

void AGrassVolumeActor::BeginPlay() {
	Super::BeginPlay();
}

void AGrassVolumeActor::GenerateBlades() {
	this->GetComponents(this->Partitions, true);
	for (int i = 0; i < this->Partitions.Num(); i++) {
		this->Partitions[i]->DestroyComponent();
	}
	this->Partitions = TArray<UGrassVolumePartition*>();

	FVector Location = this->BoundsComponent->GetComponentLocation();
	FVector Area = this->BoundsComponent->GetScaledBoxExtent();

	float PartitionXSize = Area.X / this->NumSpacePartitions;
	float PartitionYSize = Area.Y / this->NumSpacePartitions;
	for (int x = 0; x < this->NumSpacePartitions; x++) {
		for (int y = 0; y < this->NumSpacePartitions; y++) {
			FVector PartitionLocation = FVector(PartitionXSize * x, PartitionYSize * y, Location.Z);

			UGrassVolumePartition* Partition = NewObject<UGrassVolumePartition>(this);
			Partition->RegisterComponent();
			Partition->AttachToComponent(
				this->RootComponent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				FName("None")
			);
			Partition
			this->InventoryLookCollider->SetSphereRadius(0.0f);
			this->InventoryLookCollider->SetCollisionProfileName(FName("EquipHost"));
			this->InventoryLookCollider->ParentSlot = this;
		}
	}

	for (int i = 0; i < this->NumBlades; i++) {
		UStaticMeshComponent* Blade = NewObject<UStaticMeshComponent>(this);
		if (Blade == nullptr) return;

		this->AddInstanceComponent(Blade);
		Blade->RegisterComponent();
		Blade->AttachToComponent(this->BoundsComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Blade->SetStaticMesh(this->BladeMesh);
		Blade->SetCollisionProfileName(FName("NoCollision"));
		Blade->SetCastShadow(false);

		Blade->SetRelativeRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f));
		Blade->SetRelativeLocation(FVector(
			FMath::RandRange(-Area.X, Area.X),
			FMath::RandRange(-Area.Y, Area.Y),
			0.0f
		));

		this->Blades.Push(Blade);
	}
}