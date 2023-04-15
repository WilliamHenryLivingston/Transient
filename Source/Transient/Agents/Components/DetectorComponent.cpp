// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "DetectorComponent.h"

UDetectorComponent::UDetectorComponent() {
    PrimaryComponentTick.bCanEverTick = true;
}

void UDetectorComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) {
    Super::TickComponent(DeltaTime, Type, TickSelf);

    if (!this->Active || !this->HasAuthority()) return; // TODO: Actually stop ticks.

    AAgentManager* Manager = AAgentManager::AgentsGetManager(this->GetWorld());
    TArray<AAgentActor*> Hits = this->DetectorTick(DeltaTime);
    for (int i = 0; i < Hits.Num(); i++) {
        AAgentActor* Hit = Hits[i];

        // Ensure this is a valid target.
        if (!Manager->AgentsFactionsAreEnemy(this->ParentAgent, Hit, false)) continue;
        if (Target->UnitGetConcealmentScore() > this->Detection) continue;

        if (Manager->AgentAIShouldDetect(this->ParentAgent->AgentFactionID(), this->Detection, Hit)) {
            this->ParentAgent->AgentAddTarget(Hit);
            this->ParentAgent->AgentDistributeTarget(Hit);
        }
    }
}

void UDetectorComponent::DetectorSetActive(bool NewActive) {
    this->Active = NewActive;
}

TArray<AAgentActor*> UDetectorComponent::DetectorTick(float DeltaTime) {
    return TArray<AAgentActor*>();
}
