// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "TimeDilatorItem.h"

#include "Components/PointLightComponent.h"

#include "../UnitPawn.h"

void ATimeDilatorItem::BeginPlay() {
    Super::BeginPlay();

    this->Light = this->FindComponentByClass<UPointLightComponent>();
    if (this->Light != nullptr) this->Light->SetVisibility(false);
}

void ATimeDilatorItem::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    bool Active = this->CurrentHolder != nullptr && Cast<AUnitPawn>(this->CurrentHolder)->UnitIsExerted();

    if (this->Light != nullptr) this->Light->SetVisibility(Active);
}
