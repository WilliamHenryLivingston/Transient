#include "EquipItemAction.h"

#include "../AIUnit.h"

CEquipItemAction::CEquipItemAction(AItemActor* InitTarget) {
    this->Target = InitTarget;
    this->Started = false;
}

CEquipItemAction::~CEquipItemAction() {}

FAIActionExecutionResult CEquipItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (Owner->UnitGetActiveItem() != this->Target) {
        if (this->Started) return this->Unfinished; // Impossible to finish.

        Owner->UnitEquipItem(this->Target);
        this->Started = true;
        return this->Unfinished;
    }

    return this->Finished;
}
