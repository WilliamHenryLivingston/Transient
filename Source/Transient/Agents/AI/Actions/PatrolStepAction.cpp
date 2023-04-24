// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "PatrolStepAction.h"

#include "Transient/Agents/InteractiveAgent.h"

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

#if DEBUG_AI
    this->DebugInfo = FString::Printf(TEXT("patrolstep %s"), *this->Node->GetName());
#endif
}

CPatrolStepAction::~CPatrolStepAction() {}

FActionTickResult CPatrolStepAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FActionTickResult::UnfinishedAnd(CMoveToPointAction::MoveToNode(this->Node));
    }

    if (!this->EquipStarted && this->Node->EquipItem) {
        AItemActor* ToEquip = nullptr;
        if (this->Node->EquipItemType != nullptr) {
            ToEquip = Owner->UnitInventory()->InventoryFindBestItemOfClass(this->Node->EquipItemType);
            if (ToEquip == nullptr) {
                return FActionTickResult::UnfinishedAnd(new CFindItemAction(
                    Owner,
                    this->Node->EquipItemType, this->Node->FindEquipItemMaxDistance,
                    false
                ));
            }
        }

        this->EquipStarted = true;
        return FActionTickResult::UnfinishedAnd(new CEquipItemAction(ToEquip));
    }

    if (!this->InteractStarted) {
        this->InteractStarted = true;
        return FActionTickResult::UnfinishedAnd(new CInteractAction(this->Node->InteractTarget));
    }

    if (!this->UseStarted && this->Node->UseItem) {
        this->UseStarted = true;
        return FActionTickResult::UnfinishedAnd(new CUseItemAction(Owner->UnitActiveItem(), this->Node->UseItemTarget));
    }

    if (!this->SubjugateStarted) {
        if (!IsValid(this->Node->SubjugateTarget)) this->Node->SubjugateTarget = nullptr;

        this->SubjugateStarted = true;
        if (this->Node->SubjugateTarget != nullptr) {
            return FActionTickResult::UnfinishedAnd(new CSubjugateAction(this->Node->SubjugateTarget));
        }
    }

    if (!this->WaitStarted) {
        float WaitTime = FMath::RandRange(this->Node->WaitTimeMin, this->Node->WaitTimeMax);

        this->WaitStarted = true;
        return FActionTickResult::UnfinishedAnd(new CWaitAction(WaitTime, this->Node->ScanWhileWaiting));
    }

    if (this->Node->ForceNext != nullptr) {
        return FActionTickResult::UnfinishedAnd(new CPatrolStepAction(this->Node->ForceNext));
    }

    return FActionTickResult::Finished;
}
