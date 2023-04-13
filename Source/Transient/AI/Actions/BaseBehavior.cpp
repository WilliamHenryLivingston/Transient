#include "BaseBehavior.h"

#include "../AIUnit.h"
#include "../AIManager.h"
#include "MultiAction.h"
#include "FindItemAction.h"
#include "UseItemAction.h"
#include "UpdateStateAction.h"
#include "MoveToPointAction.h"
#include "AIState.h"

CBaseBehavior::CBaseBehavior(TArray<AAINavNode*>* InitPatrolSteps) {
    this->PatrolSteps = InitPatrolSteps;
    this->CurrentPatrolStep = -1;

    this->DebugInfo = TEXT("base");
}

CBaseBehavior::~CBaseBehavior() {}

FAIParentActionTickResult CBaseBehavior::AIParentActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    TSubclassOf<AItemActor> HealWithItem = nullptr;
    AI_STATE_T HealStateKey;
    bool ShouldEnergyHeal = (
        Owner->UnitGetEnergy() < 0.5f &&
        Owner->EnergyHealItem != nullptr &&
        Owner->AIState.FindOrAdd(STATE_E_HEAL, 0) == 0
    );
    if (ShouldEnergyHeal) {
        Owner->AIState.Emplace(STATE_E_HEAL, 1);

        HealWithItem = Owner->EnergyHealItem;
        HealStateKey = STATE_E_HEAL;
    }
    bool ShouldKineticHeal = (
        Owner->UnitGetKineticHealth() < 0.5f &&
        Owner->KineticHealItem != nullptr &&
        Owner->AIState.FindOrAdd(STATE_K_HEAL, 0) == 0
    );
    if (ShouldKineticHeal) {
        Owner->AIState.Emplace(STATE_K_HEAL, 1);

        HealWithItem = Owner->KineticHealItem;
        HealStateKey = STATE_K_HEAL;
    }

    if (HealWithItem != nullptr) {
        TArray<IAIActionExecutor*> Parts;
        Parts.Push(new CFindItemAction(RawOwner, HealWithItem, Owner->DefaultWorldSearchRadius, true));

        AActor* OwnerAgroTarget = Owner->AIAgroTarget();
        if (OwnerAgroTarget != nullptr) {
            AAINavNode* Cover = Manager->AIGetNavBestCoverNodeFor(Owner, OwnerAgroTarget, 30, 2000.0f);

            if (Cover != nullptr) Parts.Push(new CMoveToPointAction(Cover, Cover->ReachDistance));
        }
        Parts.Push(new CUseItemAction(nullptr, nullptr));
        Parts.Push(new CUpdateStateAction(HealStateKey, 0));

        return FAIParentActionTickResult(false, new CMultiAction(Parts));
    }

    return FAIParentActionTickResult(false, nullptr);
}

FAIActionTickResult CBaseBehavior::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    int StepCount = this->PatrolSteps->Num();
    if (StepCount == 0) return this->Unfinished;

    if (Owner->RandomizePatrol) {
        this->CurrentPatrolStep = FMath::RandRange(0, this->PatrolSteps->Num() - 1);
    }
    else {
        this->CurrentPatrolStep = (this->CurrentPatrolStep + 1) % StepCount;
    }

    return FAIActionTickResult(false, new CPatrolStepAction((*this->PatrolSteps)[this->CurrentPatrolStep]));
}
