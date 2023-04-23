// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component with a parent unit.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/Units/UnitAgent.h"

#include "AgentComponent.generated.h"

UCLASS()
class UUnitComponent : public USceneComponent {
    GENERATED_BODY()

protected:
    AUnitAgent* ParentUnit;

protected:
    virtual void BeginPlay() override;
};
