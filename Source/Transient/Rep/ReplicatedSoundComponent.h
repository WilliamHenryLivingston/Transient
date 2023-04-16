// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component encapsulating a replicated playable individual sound effect.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"

#include "ReplicatedPlayableComponent.h"

#include "ReplicatedSoundComponent.generated.h"

UCLASS()
class UReplicatedSoundComponent : public UReplicatedPlayableComponent {
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, Category="Sound")
    USoundBase* SoundEffect;

    UAudioComponent* AudioComponent;

protected:
    // Isomorphic.
    virtual void ReplicatedStartTrigger() override;
    virtual void ReplicatedStopTrigger() override;

private:
    void ReplicatedEnsureComponent();
};
