// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ReplicatedPlayableComponent.h"

UReplicatedPlayableComponent::UReplicatedPlayableComponent() {
    this->SetIsReplicated(true);
}

bool UReplicatedPlayableComponent::PlayableState() { return this->State; }

void UReplicatedPlayableComponent::PlayableStart() {
    if (this->State) return;

    this->StartCount++;

    this->ReplicatedStartTrigger();
}

void UReplicatedPlayableComponent::PlayableStop() {
    if (!this->State) return;

    this->StopCount++;

    this->ReplicatedStopTrigger();
}

void UReplicatedPlayableComponent::ReplicatedStartTrigger() {
    this->State = true;
}

void UReplicatedPlayableComponent::ReplicatedStopTrigger() {
    this->State = false;
}
