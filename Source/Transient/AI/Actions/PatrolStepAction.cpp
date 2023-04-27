#include "PatrolStepAction.h"

#include "../../Environment/InteractiveActor.h"
#include "../AIUnit.h"
#include "UseItemAction.h"
#include "EquipItemAction.h"
#include "MoveToPointAction.h"
#include "FindItemAction.h"
#include "WaitAction.h"
#include "SubjugateAction.h"
#include "InteractAction.h"

CPatrolStepAction::CPatrolStepAction(AAINavNode* InitNode) {
    this->Node = InitNode;

    this->TravelStarted = false;
    this->EquipStarted = false;
    this->WaitStarted = false;
    this->UseStarted = false;
    this->InteractStarted = false;
    this->SubjugateStarted = false;

    this->DebugInfo = FString::Printf(TEXT("patrolstep %s"), *this->Node->GetName());
}

CPatrolStepAction::~CPatrolStepAction() {}

FAIActionTickResult CPatrolStepAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FAIActionTickResult(false, new CMoveToPointAction(this->Node, this->Node->ReachDistance));
    }

    if (!this->EquipStarted) {
        if (this->Node->EquipItem) {
            this->EquipStarted = true;

            if (this->Node->EquipItemType == nullptr) {
                return FAIActionTickResult(false, new CEquipItemAction(nullptr));
            }
            else {
                AItemActor* TargetItem = Owner->UnitGetActiveItem();
                if (TargetItem == nullptr || !TargetItem->IsA(this->Node->EquipItemType)) {
                    TargetItem = Owner->UnitGetItemByClass(this->Node->EquipItemType);
                    if (TargetItem == nullptr) {
                        return FAIActionTickResult(false, new CFindItemAction(Owner, this->Node->EquipItemType, this->Node->FindEquipItemMaxDistance, false));
                    }

                    return FAIActionTickResult(false, new CEquipItemAction(TargetItem));
                }
            }
        }
    }

    if (!this->InteractStarted) {
        this->InteractStarted = true;

        AInteractiveActor* AsInteractive = Cast<AInteractiveActor>(this->Node->InteractTarget);
        if (AsInteractive != nullptr) {
            return FAIActionTickResult(false, new CInteractAction(AsInteractive));
        }
    }

    if (!this->UseStarted) {
        this->UseStarted = true;

        if (this->Node->UseItem) {
            return FAIActionTickResult(false, new CUseItemAction(Owner->UnitGetActiveItem(), this->Node->UseItemTarget));
        }
    }

    if (!this->SubjugateStarted) {
        this->SubjugateStarted = true;

        if (!IsValid(this->Node->SubjugateTarget)) this->Node->SubjugateTarget = nullptr;

        if (this->Node->SubjugateTarget != nullptr) {
            return FAIActionTickResult(false, new CSubjugateAction(this->Node->SubjugateTarget));
        }
    }

    if (!this->WaitStarted) {
        this->WaitStarted = true;

        float WaitTime = FMath::RandRange(this->Node->WaitTimeMin, this->Node->WaitTimeMax);
        return FAIActionTickResult(false, new CWaitAction(WaitTime, this->Node->ScanWhileWaiting));
    }

    if (this->Node->ForceNext != nullptr) {
        return FAIActionTickResult(true, new CPatrolStepAction(this->Node->ForceNext));
    }
    return this->Finished;
}