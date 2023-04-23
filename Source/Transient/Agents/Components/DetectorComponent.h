// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component that, when attached to a parent agent, detects enemy agents in some
// implementation-specific way.

#pragma once

#include "CoreMinimal.h"

#include "AgentComponent.h"

#include "DetectorComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UDetectorComponent : public UAgentComponent {
    GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Detector")
	int Detection;

    // Game logic.
    bool Active;

public:
    UDetectorComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

public:
    // Game logic.
    void DetectorSetActive(bool NewActive);

protected:
    virtual TArray<AAgentActor*> DetectorTick(float DeltaTime); // Returns unchecked agent hits.
};
