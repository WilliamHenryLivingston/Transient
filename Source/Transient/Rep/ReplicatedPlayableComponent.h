// Copyright: R. Saxifrage, 2023. All rights reserved.

// A base component for which static instances can replicate start and stop triggers.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"

#include "ReplicatedPlayableComponent.generated.h"

// TODO(LOW): Multi-cast instead?

UCLASS()
class UReplicatedPlayableComponent : public USceneComponent {
    GENERATED_BODY()

private:
    UPROPERTY(ReplicatedUsing=ReplicatedStartTrigger)
    int StartCount;
    UPROPERTY(ReplicatedUsing=ReplicatedStopTrigger)
    int StopCount;

    bool State;

public:
    UReplicatedPlayableComponent();

public:
    // Isomorphic.
    bool PlayableState();

    // Game logic.
    void PlayableStart();
    void PlayableStop();

protected:
    // Isomorphic.
    virtual void ReplicatedStartTrigger();
    virtual void ReplicatedStopTrigger();
};
