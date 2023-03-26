#include "UseItemAction.h"

#include "../AIUnit.h"
#include "EquipItemAction.h"

CUseItemAction::CUseItemAction(AItemActor* InitTarget) {
    this->Target = InitTarget;
    this->UseStarted = false;
}

CUseItemAction::~CUseItemAction() {}

FAIActionExecutionResult CUseItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (!this->UseStarted) {
        if (Owner->UnitGetActiveItem() != this->Target) {
            return FAIActionExecutionResult(false, new CEquipItemAction(this->Target));
        }

        Owner->UnitUseActiveItem(nullptr);
        this->UseStarted = true;
        return this->Unfinished;
    }

    return this->Finished;
}
