// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "RepairItem.h"

#include "../UnitPawn.h"

void ARepairItem::BeginPlay() {
    Super::BeginPlay();

    this->UseFX = this->FindComponentByClass<UNiagaraComponent>();
    this->UseFX->SetVisibility(false);
}

void ARepairItem::ItemStartUse() {
    this->UseFX->SetVisibility(true);
}

void ARepairItem::ItemUse(AActor* Target) {
    // TODO: Why does IItemHolder even exist?
    AUnitPawn* AsUnit = Cast<AUnitPawn>(this->CurrentHolder);

    AsUnit->UnitHealDamage(this->Healing);

    AsUnit->OverrideArmsState = true;
    AsUnit->UnitDropItem(this);
    AsUnit->OverrideArmsState = false;

    this->Destroy();
}
