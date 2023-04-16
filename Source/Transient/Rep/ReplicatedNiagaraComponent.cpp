// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ReplicatedNiagaraComponent.h"

UNiagaraComponent* UReplicatedNiagaraComponent::ReplicatedNiagaraComponent() {
    return this->NiagaraComponent;
}

void UReplicatedNiagaraComponent::ReplicatedEnsureComponent() {
    if (this->NiagaraComponent != nullptr) return;

    this->NiagaraComponent = NewObject<UNiagaraComponent>(this->GetOwner());
    this->NiagaraComponent->RegisterComponent();
    this->NiagaraComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void UReplicatedNiagaraComponent::ReplicatedStartTrigger() {
    Super::ReplicatedStartTrigger();

    this->ReplicatedEnsureComponent();

    this->NiagaraComponent->SetVisibility(true);
}

void UReplicatedNiagaraComponent::ReplicatedStopTrigger() {
    Super::ReplicatedStopTrigger();

    this->ReplicatedEnsureComponent();

    this->NiagaraComponent->SetVisibility(false);
}
