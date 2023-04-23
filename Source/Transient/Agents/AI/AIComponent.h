// Copyright: R. Saxifrage, 2023. All rights reserved.

// Manages a stack of actions, and an associated state, on behalf of the parent actor,
// which must be a unit.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/Components/UnitComponent.h"

#include "Actions/Action.h"
#include "AIState.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UAIComponent : public UUnitComponent {
    GENERATED_BODY()

private:
    // Game logic.
    TArray<CAction*> ActionStack;
    CAIState* State;

protected:
    virtual void BeginPlay() override;
    virtual void OnComponentDestroyed(bool FullHierarchy) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

public:
    void AIPushAction(CAction* Action);
};
