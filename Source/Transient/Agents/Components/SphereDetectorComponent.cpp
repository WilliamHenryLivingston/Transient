// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "SphereDetectorComponent.h"

#include "Components/SphereComponent.h"

#include "Transient/Common.h"

void USphereDetectorComponent::BeginPlay() {
    USphereComponent* Collider = NewObject<USphereComponent>(this->GetOwner());
	Collider->RegisterComponent();
	Collider->AttachToComponent(
		this,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		FName("None")
	);
	Collider->SetSphereRadius(this->DetectionRadius);
	Collider->SetCollisionProfileName(FName("OverlapAll"));
	Collider->SetHiddenInGame(!DEBUG_DETECTION);

    Collider->OnComponentBeginOverlap.AddDynamic(this, &USphereDetectorComponent::OnActorEnter);
}

TArray<AAgentActor*> USphereDetectorComponent::DetectorTick(float DeltaTime) {
    if (this->PendingHits.Num() == 0) return this->PendingHits;

    TArray<AAgentActor*> Hits = this->PendingHits;
    this->PendingHits = TArray<AAgentActor*>();

    return Hits;
}

void USphereDetectorComponent::OnActorEnter(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
    if (!this->Active) return; // TODO: Actually turn off collisions.

    AAgentActor* HitAgent = Cast<AAgentActor>(OtherActor);

    if (HitAgent != this->ParentAgent && HitAgent != nullptr) {
        this->PendingHits.Push(HitAgent);
    }
}
