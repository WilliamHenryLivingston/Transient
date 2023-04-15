// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component with a parent agent.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/AgentActor.h"

#include "AgentComponent.generated.h"

UCLASS()
class UAgentComponent : public USceneComponent {
    GENERATED_BODY()

protected:
    AAgentActor* ParentAgent;

protected:
    virtual void BeginPlay() override;
};
