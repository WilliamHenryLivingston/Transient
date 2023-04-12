#include "EquipItemAction.h"

#include "../AIUnit.h"

CEquipItemAction::CEquipItemAction(AItemActor* InitTarget) {
    this->Target = InitTarget;
    this->Started = false;

    FString Name = TEXT("<nothing>");
    if (this->Target != nullptr) {
        Name = this->Target->GetName();
    }
    this->DebugInfo = FString::Printf(TEXT("equip %s"), *Name);
}

CEquipItemAction::~CEquipItemAction() {}

FAIActionTickResult CEquipItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (Owner->UnitGetActiveItem() != this->Target) {
        if (this->Started) return this->Unfinished; // Impossible to finish.

        if (this->Target == nullptr) Owner->UnitDequipActiveItem();
        else Owner->UnitEquipItem(this->Target);

        this->Started = true;
        return this->Unfinished;
    }

    return this->Finished;
}
