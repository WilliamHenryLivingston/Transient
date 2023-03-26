#include "PatrolStepAIAction.h"

#include "../AIUnit.h"
#include "UseItemAction.h"
#include "EquipItemAction.h"
#include "MoveToPointAction.h"

CPatrolStepAIAction::CPatrolStepAIAction(FAIPatrolStep InitStep) {
    this->Step = InitStep;

    this->Timer = 0.0f;
    this->ScanTimer = -1.0f;
    this->TravelStarted = false;
    this->UseFinished = false;
    this->RemainingItemUseTimes = this->Step.UseItemTimes;
}

CPatrolStepAIAction::~CPatrolStepAIAction() {}

FAIActionExecutionResult CPatrolStepAIAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FAIActionExecutionResult(false, new CMoveToPointAction(this->Step.Node, this->Step.NodeReachDistance));
    }

    if (!this->UseFinished) {
        if (this->Step.EquipItem) {
            AItemActor* TargetItem = Owner->UnitGetActiveItem();
            if (TargetItem == nullptr || !TargetItem->IsA(this->Step.EquipItemType)) {
                return FAIActionExecutionResult(false, new CEquipItemAction(Owner->UnitGetItemByClass(this->Step.EquipItemType)));
            }
        }

        if (this->RemainingItemUseTimes < 0) {
            this->UseFinished = true;
            this->Timer = FMath::RandRange(this->Step.WaitTimeMin, this->Step.WaitTimeMax);
            return this->Unfinished;
        }
        else {
            this->RemainingItemUseTimes--;
            if (this->RemainingItemUseTimes >= 0) {
                return FAIActionExecutionResult(false, new CUseItemAction(Owner->UnitGetActiveItem()));
            }

            return this->Unfinished;
        }
    }

    if (this->Timer > 0.0f) {
        this->Timer -= DeltaTime;

        if (this->ScanTimer > 0.0f) {
            this->ScanTimer -= DeltaTime;
        }
        else {
            FRotator NewRotation;
            NewRotation.Yaw = FMath::RandRange(0.0f, 360.0f);

            this->ScanVector = Owner->GetActorLocation() + NewRotation.RotateVector(FVector(-100.0f, 0.0f, 0.0f));
            this->ScanTimer = FMath::RandRange(3.0f, 5.0f);
        }
        Owner->UnitFaceTowards(this->ScanVector);

        return this->Unfinished;
    }

    return this->Finished;
}
