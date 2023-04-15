// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentComponent.h"

void UAgentComponent::BeginPlay() {
    Super::BeginPlay();

    this->ParentAgent = Cast<AAgentActor>(this->GetOwner());
}
