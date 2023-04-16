// Copyright: R. Saxifrage, 2023. All rights reserved.

// A replicated actor that plays a one-shot particle system effect with sound.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"

#include "ReplicatedEffectActor.generated.h"

UCLASS()
class AReplicatedEffectActor : public AActor {
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category="Effect")
    float LifeTime;
    UPROPERTY(EditDefaultsOnly, Category="Effect")
    USoundBase* EffectSound;
    UPROPERTY(EditDefaultsOnly, Category="Effect")
    UNiagaraSystem* EffectNiagara;

    // Cosmetic.
    float LifeTimer;

    UAudioComponent* SoundComponent;
    UNiagaraComponent* NiagaraComponent;

public:
    AReplicatedEffectActor();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
};
