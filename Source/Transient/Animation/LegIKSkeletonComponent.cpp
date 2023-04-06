// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKSkeletonComponent.h"

void ULegIKSkeletonComponent::BeginPlay() {
    Super::BeginPlay();

    this->IK = Cast<ULegIKInstance>(this->GetAnimInstance());
    this->IK->LegIKInstanceInit(this, this->InstanceConfig, this->TracksConfig);
}

void ULegIKSkeletonComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) {
    Super::TickComponent(DeltaTime, Type, TickSelf);
    
    if (this->IK != nullptr) this->IK->LegIKInstanceTick(DeltaTime, this);
}
