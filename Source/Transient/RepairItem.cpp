#include "RepairItem.h"

void ARepairItem::ItemUse(AActor* Target) {
    this->CurrentHolder->UnitHealDamage(this->Healing);

    this->CurrentHolder->OverrideArmsState = true;
    this->CurrentHolder->UnitDropItem(this);
    this->CurrentHolder->OverrideArmsState = false;

    this->Destroy();
}
