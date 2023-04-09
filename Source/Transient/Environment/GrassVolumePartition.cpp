// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "GrassVolumePartition.h"

#include "Kismet/KismetMathLibrary.h"

#include "../Items/ItemActor.h"
#include "GrassVolumeActor.h"

//#define DEBUG_DRAWS true

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
	this->OnComponentBeginOverlap.AddDynamic(this, &UGrassVolumePartition::OnActorEnter);
	this->OnComponentEndOverlap.AddDynamic(this, &UGrassVolumePartition::OnActorLeave);

	this->GetOverlappingActors(this->TrackedActors, AItemActor::StaticClass());
}

void UGrassVolumePartition::TickComponent(
    float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf
) {
	#ifdef DEBUG_DRAWS
	this->SetVisibility(this->TrackedActors.Num() > 0);
	this->SetHiddenInGame(this->TrackedActors.Num() == 0);
	#endif

	if (this->TrackedActors.Num() == 0 && this->FullyInert) return;

	AGrassVolumeActor* ParentVolume = Cast<AGrassVolumeActor>(this->GetOwner());

	this->FullyInert = true;
	for (int i = 0; i < this->Blades.Num(); i++) {
		USceneComponent* Blade = this->Blades[i];
		FVector BladeLocation = Blade->GetComponentLocation();
		FRotator BladeRotation = Blade->GetRelativeRotation();

		AActor* NearbyActor = nullptr;
		float NearbyDist = 150.0f;
		for (int j = 0; j < this->TrackedActors.Num(); j++) {
			AActor* Check = this->TrackedActors[j];
			if (!IsValid(Check)) {
				this->TrackedActors.RemoveAt(j);
				j--;
				continue;
			}

			float Dist = (Check->GetActorLocation() - BladeLocation).Size();
			if (Dist < NearbyDist) {
				NearbyActor = Check;
				NearbyDist = Dist;
			}
		}

		FRotator BaseRotation = FRotator(0.0f, this->Yaws[i], 0.0f);
		FRotator Target = BaseRotation;
		float ISpeed = 2.0f;
		if (NearbyActor != nullptr) {
			ISpeed = 20.0f;
			Target = UKismetMathLibrary::FindLookAtRotation(BladeLocation, NearbyActor->GetActorLocation());
		
			if (ParentVolume->CrushAlpha < 1.0f) {
				Target = FMath::Lerp(BaseRotation, Target, ParentVolume->CrushAlpha);
			}
		}

		Blade->SetRelativeRotation(FMath::RInterpTo(BladeRotation, Target, DeltaTime, ISpeed));
		if (!Blade->GetRelativeRotation().Equals(Target, 10.0f)) this->FullyInert = false;
	}
}

void UGrassVolumePartition::OnActorEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Push(OtherActor);
}

void UGrassVolumePartition::OnActorLeave(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Remove(OtherActor);
}
