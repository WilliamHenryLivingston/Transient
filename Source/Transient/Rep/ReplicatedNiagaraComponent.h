// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component encapsulating a replicated playable individual niagara system.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "ReplicatedPlayableComponent.h"

#include "ReplicatedNiagaraComponent.generated.h"

UCLASS()
class UReplicatedNiagaraComponent : public UReplicatedPlayableComponent {
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, Category="Sound")
    UNiagaraSystem* System;

    UNiagaraComponent* NiagaraComponent;

public:
    // Isomorphic.
    UNiagaraComponent* ReplicatedNiagraComponent();

protected:
    virtual void ReplicatedStartTrigger() override;
    virtual void ReplicatedStopTrigger() override;

private:
    void ReplicatedEnsureComponent();
};
