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

	float PartitionSize = this->AreaSize / this->NumSpacePartitions;
	float HalfAreaSize = this->AreaSize / 2.0f;
	float Padding = 30.0f;

	TArray<UGrassVolumePartition*> CreatedPartitions;
	for (int x = 0; x < this->NumSpacePartitions; x++) {
		for (int y = 0; y < this->NumSpacePartitions; y++) {
			UGrassVolumePartition* Partition = NewObject<UGrassVolumePartition>(this);
			this->AddInstanceComponent(Partition);
			Partition->RegisterComponent();

			float PartitionX = Location.X - this->AreaSize + Padding + (PartitionSize * (x + 0.5f) * 2);
			float PartitionY = Location.Y - this->AreaSize + Padding + (PartitionSize * (y + 0.5f) * 2);
			Partition->SetRelativeLocation(FVector(PartitionX, PartitionY, Location.Z));
			Partition->SetBoxExtent(FVector(PartitionSize + (Padding * 2.0f), PartitionSize + (Padding * 2.0f), 30.0f), false);
			Partition->SetVisibility(false);

			CreatedPartitions.Push(Partition);
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

		float Angle = FMath::RandRange(0.0f, 360.0f);
		float Radius = FMath::RandRange(0.0f, 1.0f) * HalfAreaSize;
		FVector BladeLocation = FRotator(0.0f, Angle, 0.0f).RotateVector(FVector(Radius, 0.0f, 0.0f));
		Blade->SetRelativeLocation(BladeLocation * 2.0f); // ???.
		Blade->SetRelativeRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f));
		Blade->SetRelativeScale3D(FVector(1.0f, 1.0f, FMath::RandRange(this->HeightScaleMin, this->HeightScaleMax)));

		int XIndex = (BladeLocation.X + HalfAreaSize) / PartitionSize;
		int YIndex = (BladeLocation.Y + HalfAreaSize) / PartitionSize;
		UGrassVolumePartition* ParentPartition = CreatedPartitions[(XIndex * this->NumSpacePartitions) + YIndex];
		Blade->AttachToComponent(
			ParentPartition,
			FAttachmentTransformRules::KeepWorldTransform,
			FName("None")
		);
	}
}