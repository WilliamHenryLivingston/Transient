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
                if (this->Step.EquipItemType != nullptr) {
                    TargetItem = Owner->UnitGetItemByClass(this->Step.EquipItemType);
                    if (TargetItem == nullptr) {
                        TargetItem = Owner->GetWorld()->SpawnActor<AItemActor>(
                            this->Step.EquipItemType,
                            Owner->GetActorLocation(),
                            Owner->GetActorRotation(),
                            FActorSpawnParameters()
                        );
                        Owner->OverrideArmsState = true;
                        Owner->UnitTakeItem(TargetItem);
                        Owner->OverrideArmsState = false;
                    }
                }

                return FAIActionTickResult(false, new CEquipItemAction(TargetItem));
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
