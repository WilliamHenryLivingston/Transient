// Copyright: R. Saxifrage, 2023. All rights reserved.

// A detector component that performs ray casts relative to component rotation.

#pragma once

#include "CoreMinimal.h"

#include "DetectorComponent.h"

#include "RayCastDetectorComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class URayCastDetectorComponent : public UDetectorComponent {
    GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Detector")
	float DetectionDistance;
	UPROPERTY(EditDefaultsOnly, Category="Detector")
    int RayCount;
	UPROPERTY(EditDefaultsOnly, Category="Detector")
    FRotator RayOffset;

protected:
    // Game logic.
    virtual TArray<AAgentActor*> DetectorTick(float DeltaTime) override;
};
