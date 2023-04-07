// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "GrassVolumePartition.h"

#include "Kismet/KismetMathLibrary.h"

UGrassVolumePartition::UGrassVolumePartition() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrassVolumePartition::BeginPlay() {
	this->GetChildrenComponents(false, this->Blades);

	this->FullyInert = true;

	for (int i = 0; i < this->Blades.Num(); i++) {
		this->Yaws.Push(this->Blades[i]->GetRelativeRotation().Yaw);
	}

	this->SetCollisionProfileName(FName("Grass"));
	this->OnComponentBeginOverlap.AddDynamic(this, &UGrassVolumePartition::OnPawnEnter);
	this->OnComponentEndOverlap.AddDynamic(this, &UGrassVolumePartition::OnPawnLeave);
}

void UGrassVolumePartition::TickComponent(
    float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf
) {
	if (this->TrackedActors.Num() == 0 && this->FullyInert) return;

	this->FullyInert = true;
	for (int i = 0; i < this->Blades.Num(); i++) {
		USceneComponent* Blade = this->Blades[i];
		FVector BladeLocation = Blade->GetComponentLocation();

		AActor* NearbyActor = nullptr;
		float NearbyDist = 150.0f;
		for (int j = 0; j < this->TrackedActors.Num(); j++) {
			AActor* Check = this->TrackedActors[j];
			if (!IsValid(Check)) continue; // TODO: Better.

			float Dist = (Check->GetActorLocation() - BladeLocation).Size();
			if (Dist < NearbyDist) {
				NearbyActor = Check;
				NearbyDist = Dist;
			}
		}

		FRotator Target = FRotator(0.0f, this->Yaws[i], 0.0f);
		float ISpeed = 2.0f;
		if (NearbyActor != nullptr) {
			ISpeed = 20.0f;
			Target = UKismetMathLibrary::FindLookAtRotation(BladeLocation, NearbyActor->GetActorLocation());
		}

		Blade->SetRelativeRotation(FMath::RInterpTo(Blade->GetRelativeRotation(), Target, DeltaTime, ISpeed));
		if (!Blade->GetRelativeRotation().Equals(Target, 10.0f)) this->FullyInert = false;
	}
}

void UGrassVolumePartition::OnPawnEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Push(OtherActor);
}

void UGrassVolumePartition::OnPawnLeave(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Remove(OtherActor);
}
