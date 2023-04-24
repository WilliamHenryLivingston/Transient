// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "UnitSkeletonComponent.h"

void UUnitSkeletonComponent::BeginPlay() {
    Super::BeginPlay();

    this->ParentUnit = Cast<UUnitAgent>(this->GetOwner());
    this->LegIK = Cast<ULegIKInstance>(this->GetAnimInstance());
}

void UUnitSkeletonComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) {
    Super::TickComponent(DeltaTime, Type, TickSelf);

    this->LegIK->LegIKInstanceTick(DeltaTime, this);
}
