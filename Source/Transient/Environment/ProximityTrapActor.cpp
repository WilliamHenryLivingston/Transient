// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ProximityTrapActor.h"

#include "Kismet/KismetMathLibrary.h"

#include "../AI/AIManager.h"

AProximityTrapActor::AProximityTrapActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void AProximityTrapActor::BeginPlay() {
	Super::BeginPlay();

	this->TriggerComponent = this->FindComponentByClass<USphereComponent>();
	this->VisibleComponent = this->FindComponentByClass<UStaticMeshComponent>();

	this->TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AProximityTrapActor::OnActorEnter);
	this->TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AProximityTrapActor::OnActorLeave);
}

void AProximityTrapActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (this->ActivationTimer > 0.0f) {
		this->ActivationTimer -= DeltaTime;

		// TODO: Different effects.
		FVector Location = this->VisibleComponent->GetRelativeLocation();
		Location.Z += DeltaTime * this->RiseSpeed;
		this->VisibleComponent->SetRelativeLocation(Location);

		for (int i = 0; i < this->ActiveTargets.Num(); i++) {
			IDamagable* ThisTarget = this->ActiveTargets[i];
			AActor* AsActor = Cast<AActor>(ThisTarget);
			if (!IsValid(AsActor)) continue;

			ThisTarget->DamagableTakeDamage(this->Damage, nullptr);
			if (this->HitEffect != nullptr) {
				FVector CurrentLocation = this->GetActorLocation();
				FVector OtherLocation = AsActor->GetActorLocation();

				FVector PlanarCurrentLocation = CurrentLocation;
				PlanarCurrentLocation.Z = OtherLocation.Z;
				this->GetWorld()->SpawnActor<AActor>(
					this->HitEffect,
					OtherLocation,
					UKismetMathLibrary::FindLookAtRotation(OtherLocation, PlanarCurrentLocation),
					FActorSpawnParameters()
				);
			}
		}
		this->Destroy();
	}
}

void AProximityTrapActor::DamagableTakeDamage(FDamageProfile Profile, AActor* Source) {
	this->KineticHealth -= Profile.Kinetic;

	if (this->KineticHealth <= 0.0f) {
		this->TrapDetonate();
	}
}

void AProximityTrapActor::TrapDetonate() {
	this->ActivationTimer = this->ActivationTime;

	if (this->TriggerEffect != nullptr) {
		FVector CurrentLocation = this->GetActorLocation();
		this->GetWorld()->SpawnActor<AActor>(
			this->TriggerEffect,
			CurrentLocation,
			FRotator(),
			FActorSpawnParameters()
		);
	}
}

void AProximityTrapActor::OnActorEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (OtherActor == nullptr) return;

	AUnitPawn* AsPawn = Cast<AUnitPawn>(OtherActor);

	AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());

	this->ActiveTargets.Push(AsPawn);

	if (AsPawn != nullptr && Manager->AIIsFactionEnemy(this->FactionID, AsPawn->FactionID)) {
		this->TrapDetonate();
	}
}

void AProximityTrapActor::OnActorLeave(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	if (OtherActor == nullptr) return;

	IDamagable* AsDamagable = Cast<IDamagable>(OtherActor);

	if (AsDamagable != nullptr) this->ActiveTargets.Remove(AsDamagable);
}
