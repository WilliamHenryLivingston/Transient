// Copyright: R. Saxifrage, 2023. All rights reserved.

// A detector component that uses a sphere collider to detect nearby agents.

#pragma once

#include "CoreMinimal.h"

#include "DetectorComponent.h"

#include "SphereDetectorComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class USphereDetectorComponent : public UDetectorComponent {
    GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Detector")
	float DetectionRadius;

    // Game logic.
    TArray<AAgentActor*> PendingHits;

protected:
    virtual void BeginPlay() override;

protected:
    // Game logic.
    virtual TArray<AAgentActor*> DetectorTick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnActorEnter(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
};
