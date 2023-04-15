// Copyright: R. Saxifrage, 2023. All rights reserved.

// A component that toggles the visibility of its children depending on whether
// its parent agent has targets.

#pragma once

#include "CoreMinimal.h"

#include "AgentComponent.h"

#include "TargetStateGroupComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UTargetStateGroupComponent : public UAgentComponent {
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, Category="Alert Component Group")
    bool ActiveWhenTargetsExist;

    bool LastActive; // No need to replicate; underlying check on agent is replicated.

public:
    UTargetStateGroupComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;
};
