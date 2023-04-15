// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "TargetStateGroupComponent.h"

UTargetStateGroupComponent::UTargetStateGroupComponent() {
    PrimaryComponentTick.bCanEverTick = true;
}

void UTargetStateGroupComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) {
    Super::TickComponent(DeltaTime, Type, TickSelf);

    bool Active = this->ParentAgent->AgentHasTargets() == this->ActiveWhenTargetsExist;

    if (Active != this->LastActive) {
        TArray<USceneComponent*> Children;
        this->GetChildrenComponents(true, Children);

        for (int i = 0; i < Children.Num(); i++) {
            Children[i]->SetVisibility(Active); // TODO: Stop ticks as well?
        }
    }

    this->LastActive = Active;
}
