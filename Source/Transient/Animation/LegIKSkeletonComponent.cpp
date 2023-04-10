// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKSkeletonComponent.h"

#include "../Debug.h"
#include "LegIKProfiles.h"

void ULegIKSkeletonComponent::BeginPlay() {
    Super::BeginPlay();

    this->IK = Cast<ULegIKInstance>(this->GetAnimInstance());

    if (this->ProfileType == ELegIKProfileType::Biped) {
        this->IK->LegIKInstanceInit(this, FBipedLegIKProfile());
    }
    else {
        this->IK = nullptr;
        ERR_LOG(this->GetOwner(), "invalid IK profile!");
    }
}

void ULegIKSkeletonComponent::TickComponent(
    float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf
) {
    Super::TickComponent(DeltaTime, Type, TickSelf);

    if (this->IK != nullptr) this->IK->LegIKInstanceTick(DeltaTime, this);
}

void ULegIKSkeletonComponent::LegIKSetDynamics(FLegIKDynamics Dynamics) {
    if (this->IK != nullptr) this->IK->LegIKInstanceSetDynamics(Dynamics);
}
