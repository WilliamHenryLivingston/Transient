// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "UnitComponent.h"

void UUnitComponent::BeginPlay() {
    Super::BeginPlay();

    this->ParentUnit = Cast<AUnitAgent>(this->GetOwner());
}
