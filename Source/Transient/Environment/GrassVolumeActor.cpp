// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "GrassVolumeActor.h"

AGrassVolumeActor::AGrassVolumeActor() {
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->BoundsComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
}

void AGrassVolumeActor::BeginPlay() {
	Super::BeginPlay();
}

void AGrassVolumeActor::GenerateBlades() {
	TArray<UStaticMeshComponent*> PrevBlades;
	this->GetComponents(PrevBlades, true);
	for (int i = 0; i < PrevBlades.Num(); i++) {
		PrevBlades[i]->DestroyComponent();
	}

	this->GetComponents(this->Partitions, true);
	for (int i = 0; i < this->Partitions.Num(); i++) {
		this->Partitions[i]->DestroyComponent();
	}
	this->Partitions = TArray<UGrassVolumePartition*>();

	FVector Location = this->BoundsComponent->GetComponentLocation();
	FVector Area = this->BoundsComponent->GetScaledBoxExtent();

	float PartitionXSize = Area.X / this->NumSpacePartitions;
	float PartitionYSize = Area.Y / this->NumSpacePartitions;
	int BladesPer = this->NumBlades / this->NumSpacePartitions;

	for (int x = 0; x < this->NumSpacePartitions; x++) {
		for (int y = 0; y < this->NumSpacePartitions; y++) {
			UGrassVolumePartition* Partition = NewObject<UGrassVolumePartition>(this);
			this->AddInstanceComponent(Partition);
			Partition->RegisterComponent();

			float PartitionX = Location.X - Area.X + (PartitionXSize * (x + 0.5f) * 2);
			float PartitionY = Location.Y - Area.Y + (PartitionYSize * (y + 0.5f) * 2);
			Partition->SetRelativeLocation(FVector(PartitionX, PartitionY, Location.Z));
			Partition->SetBoxExtent(FVector(PartitionXSize, PartitionYSize, Area.Z), false);

			for (int i = 0; i < BladesPer; i++) {
				UStaticMeshComponent* Blade = NewObject<UStaticMeshComponent>(this);
				if (Blade == nullptr) return;

				this->AddInstanceComponent(Blade);
				Blade->RegisterComponent();
				Blade->AttachToComponent(this->BoundsComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
				Blade->SetStaticMesh(this->BladeMesh);
				Blade->SetCollisionProfileName(FName("NoCollision"));
				Blade->SetCastShadow(false);
				Blade->AttachToComponent(Partition, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("None"));

				Blade->SetRelativeRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f));
				Blade->SetRelativeLocation(FVector(
					FMath::RandRange(-PartitionXSize, PartitionXSize),
					FMath::RandRange(-PartitionYSize, PartitionYSize),
					0.0f
				));
			}
		}
	}

	for (int i = 0; i < this->NumBlades; i++) {
		
	}
}