#include "ProximityTrapActor.h"

#include "TransientDebug.h"
#include "AIManager.h"

AProximityTrapActor::AProximityTrapActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void AProximityTrapActor::BeginPlay() {
	Super::BeginPlay();

	this->TriggerComponent = this->FindComponentByClass<USphereComponent>();
	this->VisibleComponent = this->FindComponentByClass<UStaticMeshComponent>();

	this->TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AProximityTrapActor::OnUnitEnterUnchecked);
	this->TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AProximityTrapActor::OnUnitLeaveUnchecked);
}

void AProximityTrapActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (this->ActivationTimer > 0.0f) {
		this->ActivationTimer -= DeltaTime;

		// TODO: Different effects.
		FVector Location = this->VisibleComponent->GetRelativeLocation();
		Location.Z += DeltaTime * this->RiseSpeed;
		this->VisibleComponent->SetRelativeLocation(Location);

		if (this->ActivationTimer <= 0.0f && this->ActiveTarget != nullptr && IsValid(this->ActiveTarget)) {
			this->ActiveTarget->UnitTakeDamage(this->Damage, nullptr);
			
			this->Destroy();
		}
	}
}

void AProximityTrapActor::OnUnitEnterUnchecked(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (OtherActor == nullptr) return;

	AUnitPawn* AsPawn = Cast<AUnitPawn>(OtherActor);

	AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());

	if (AsPawn != nullptr && Manager->AIIsFactionEnemy(this->FactionID, AsPawn->FactionID)) {
		this->ActivationTimer = this->ActivationTime;
		this->ActiveTarget = AsPawn;
	}
}

void AProximityTrapActor::OnUnitLeaveUnchecked(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	if (OtherActor == nullptr) return;

	AUnitPawn* AsPawn = Cast<AUnitPawn>(OtherActor);

	if (AsPawn == this->ActiveTarget) {
		this->ActiveTarget = nullptr;
	}
}
