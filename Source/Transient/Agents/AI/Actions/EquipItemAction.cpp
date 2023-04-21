// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "EquipItemAction.h"

CEquipItemAction::CEquipItemAction(AItemActor* InitTarget) {
    this->Target = InitTarget;
    this->Started = false;

#if DEBUG_ACTIONS
    FString Name = TEXT("<nothing>");
    if (this->Target != nullptr) Name = this->Target->GetName();
    this->DebugInfo = FString::Printf(TEXT("equip %s"), *Name);
#endif
}

CEquipItemAction::~CEquipItemAction() {}

FActionTickResult CEquipItemAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (Owner->UnitArmsOccupied()) return FActionTickResult::Unfinished;

    if (Owner->UnitActiveItem() != this->Target) {
        if (this->Started) return FActionTickResult::Error(0);

        if (this->Target == nullptr) Owner->UnitDequipActiveItem();
        else Owner->UnitEquipItem(this->Target);

        this->Started = true;
        return FActionTickResult::Unfinished;
    }

    return FActionTickResult::Finished;
}
