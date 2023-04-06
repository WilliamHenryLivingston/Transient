// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "GrassVolumeActor.h"

#include "Kismet/KismetMathLibrary.h"

AGrassVolumeActor::AGrassVolumeActor() {
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->BoundsComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
}

void AGrassVolumeActor::BeginPlay() {
	Super::BeginPlay();

	this->BoundsComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrassVolumeActor::OnPawnEnter);
	this->BoundsComponent->OnComponentEndOverlap.AddDynamic(this, &AGrassVolumeActor::OnPawnLeave);

	this->GetComponents(this->Blades, true);

	for (int i = 0; i < this->Blades.Num(); i++) {
		this->Yaws.Push(this->Blades[i]->GetRelativeRotation().Yaw);
	}
}

void AGrassVolumeActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// TODO: Optimize haha.
	for (int i = 0; i < this->Blades.Num(); i++) {
		UStaticMeshComponent* Blade = this->Blades[i];
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
	}
}

void AGrassVolumeActor::OnPawnEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Push(OtherActor);
}

void AGrassVolumeActor::OnPawnLeave(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	if (OtherActor == nullptr) return;

	this->TrackedActors.Remove(OtherActor);
}

void AGrassVolumeActor::GenerateBlades() {
	for (int i = 0; i < this->Blades.Num(); i++) {
		this->Blades[i]->DestroyComponent();
	}
	this->Blades = TArray<UStaticMeshComponent*>();

	FVector Area = this->BoundsComponent->GetScaledBoxExtent();

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