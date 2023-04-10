// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ConcealmentActor.h"

#include "Components/ShapeComponent.h"

#include "../UnitPawn.h"
#include "../Debug.h"

void AConcealmentActor::BeginPlay() {
	Super::BeginPlay();

	UShapeComponent* Collider = this->FindComponentByClass<UShapeComponent>();
	if (Collider == nullptr) {
		ERR_LOG(this, "no collider component");
		return;
	}

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AConcealmentActor::OnActorEnter);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AConcealmentActor::OnActorLeave);
}

void AConcealmentActor::OnActorEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	AUnitPawn* AsUnit = Cast<AUnitPawn>(OtherActor);
	if (AsUnit == nullptr) return;

	AsUnit->UnitAddConcealment(this, this->Score, this->ScoreCrouched);
}

void AConcealmentActor::OnActorLeave(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
) {
	AUnitPawn* AsUnit = Cast<AUnitPawn>(OtherActor);
	if (AsUnit == nullptr) return;

	AsUnit->UnitRemoveConcealment(this);
}
