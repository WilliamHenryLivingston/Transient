#include "PatrolStepAction.h"

#include "../AIUnit.h"
#include "UseItemAction.h"
#include "EquipItemAction.h"
#include "MoveToPointAction.h"
#include "WaitAction.h"

CPatrolStepAction::CPatrolStepAction(FAIPatrolStep InitStep) {
    this->Step = InitStep;

    this->TravelStarted = false;
    this->UseFinished = false;
    this->RemainingItemUseTimes = this->Step.UseItemTimes;
}

CPatrolStepAction::~CPatrolStepAction() {}

FAIActionTickResult CPatrolStepAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FAIActionTickResult(false, new CMoveToPointAction(this->Step.Node, this->Step.NodeReachDistance));
    }

    if (!this->UseFinished) {
        if (this->Step.EquipItem) {
            AItemActor* TargetItem = Owner->UnitGetActiveItem();
            if (TargetItem == nullptr || !TargetItem->IsA(this->Step.EquipItemType)) {
                return FAIActionTickResult(false, new CEquipItemAction(Owner->UnitGetItemByClass(this->Step.EquipItemType)));
            }
        }

        if (this->RemainingItemUseTimes < 0) {
            this->UseFinished = true;
            return this->Unfinished;
        }
        else {
            this->RemainingItemUseTimes--;
            if (this->RemainingItemUseTimes >= 0) {
                return FAIActionTickResult(false, new CUseItemAction(Owner->UnitGetActiveItem(), this->Step.UseItemTarget));
            }

            return this->Unfinished;
        }
    }

    float WaitTime = FMath::RandRange(this->Step.WaitTimeMin, this->Step.WaitTimeMax);
    return FAIActionTickResult(true, new CWaitAction(WaitTime, this->Step.ScanWhileWaiting));
}
