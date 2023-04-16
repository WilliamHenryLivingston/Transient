// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ReplicatedSoundComponent.h"

void UReplicatedSoundComponent::ReplicatedEnsureComponent() {
    if (this->AudioComponent != nullptr) return;

    this->AudioComponent = NewObject<UAudioComponent>(this->GetOwner());
    this->AudioComponent->RegisterComponent();
    this->AudioComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void UReplicatedSoundComponent::ReplicatedStartTrigger() {
    Super::ReplicatedStartTrigger();

    this->ReplicatedEnsureComponent();

    this->AudioComponent->PitchMultiplier = this->GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

    this->AudioComponent->Sound = this->SoundEffect;
    this->AudioComponent->Play(0.0f);
}

void USoundEmitterComponent::ReplicatedStopTrigger() {
    Super::ReplicatedStopTrigger();

    this->ReplicatedEnsureComponent();

    this->AudioComponent->Stop();
}
