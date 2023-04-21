// Copyright: R. Saxifrage, 2023. All rights reserved.

// Manages a stack of actions, and an associated state, on behalf of the parent actor,
// which must be a unit.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/Units/UnitAgent.h"

#include "Actions/Action.h"
#include "AIState.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class AIComponent : public USceneComponent {
    GENERATED_BODY()

private:
    // Game logic.
    AUnitAgent* ParentAgent;
    TArray<CAction*> ActionStack;
    CAIState* State;

protected:
    virtual void BeginPlay() override;
    virtual void OnComponentDestroyed(bool FullHierarchy) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

public:
    void PushAction(CAction* Action);
};
