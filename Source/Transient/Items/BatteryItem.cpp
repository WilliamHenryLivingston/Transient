// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BatteryItem.h"

#include "Kismet/KismetMathLibrary.h"

#include "../UnitPawn.h"

void ABatteryItem::ItemUse(AActor* Target) {
    // TODO: Why does IItemHolder even exist?
    AUnitPawn* AsUnit = Cast<AUnitPawn>(this->CurrentHolder);

    AsUnit->UnitHealDamage(this->Healing);
    if (this->FinishEffect != nullptr) {
        FVector CurrentLocation = this->GetActorLocation();
        this->GetWorld()->SpawnActor<AActor>(
            this->FinishEffect,
            CurrentLocation,
            UKismetMathLibrary::FindLookAtRotation(AsUnit->GetActorLocation(), CurrentLocation),
            FActorSpawnParameters()
        );
    }

    AsUnit->OverrideArmsState = true;
    AsUnit->UnitDropItem(this);
    AsUnit->OverrideArmsState = false;

    this->Destroy();
}
