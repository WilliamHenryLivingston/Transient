// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BatteryItem.h"

#include "Kismet/KismetMathLibrary.h"

#include "Transient/Agents/Units/UnitAgent.h"

void ABatteryItem::ItemUse_Implementation(AActor* Target) {
    Super::ItemUse(Target);

    AUnitAgent* Holder = this->ItemHolderUnit();

    Holder->UnitHealDamage(this->Healing);
    if (this->FinishEffect != nullptr) {
        FVector CurrentLocation = this->GetActorLocation();
        this->GetWorld()->SpawnActor<AActor>(
            this->FinishEffect,
            CurrentLocation,
            UKismetMathLibrary::FindLookAtRotation(Holder->GetActorLocation(), CurrentLocation),
            FActorSpawnParameters()
        );
    }

    this->Destroy();
}
