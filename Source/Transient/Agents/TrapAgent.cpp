// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ProximityTrapActor.h"

#include "Kismet/KismetSystemLibrary.h"

ATrapAgent::ATrapAgent() {
	PrimaryActorTick.bCanEverTick = true;
}

void ATrapAgent::Destroyed() {
	Super::Destroyed();

	if (this->DetonateEffect != nullptr) {
		this->GetWorld()->SpawnActor<AActor>(
			this->DetonateEffect,
			Location,
			FRotator(0.0f, 0.0f, 0.0f),
			FActorSpawnParameters()
		);
	}
}

void ATrapAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!this->HasAuthority()) return;

	if (this->DetontationTimer <= 0.0f) return;

	FVector Location = this->GetActorLocation();

	this->DetontationTimer -= DeltaTime;
	if (this->DetontationTimer <= 0.0f) {
		TArray<AActor*> Hits;
		UKismetSystemLibrary::SphereOverlapActors(
			this->GetWorld(),
			Location,
			this->DetonationRadius,
			TArray<EObjectTypeQuery>(),
			nullptr,
			TArray<AActor*>(),
			Hits
		);

		for (int i = 0; i < Hits.Num(); i++) {
			IDamagable* AsDamagable = Cast<IDamagable>(Hits[i]);
			if (AsDamagable == nullptr) continue;

			AsDamagable->DamagableTakeDamage(this->Damage, this, this);
		}

		this->Destroy();
	}
}

void ATrapAgent::AgentAddTarget(AAgentActor* Target) {
	Super::AgentAddTarget(Target);

	this->ProximityTrapDetonate();
}

void ATrapAgent::DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) {
	this->KineticHealth -= Profile.Kinetic;

	if (this->KineticHealth <= 0.0f) this->ProximityTrapDetonate();
}

void ATrapAgent::ProximityTrapDetonate() {
	if (this->Detonating) return;

	this->DetontationTimer = this->DetontationTime;
	this->Detonating = true;

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

void ATrapAgent::ProximityTrapDetonatingChanged_Implementation() { }
