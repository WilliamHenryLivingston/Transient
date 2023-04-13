#include "BaseBehavior.h"

#include "../AIUnit.h"
#include "MultiAction.h"
#include "FindItemAction.h"
#include "UseItemAction.h"
#include "UpdateStateAction.h"
#include "AIState.h"

CBaseBehavior::CBaseBehavior(TArray<AAINavNode*>* InitPatrolSteps) {
    this->PatrolSteps = InitPatrolSteps;
    this->CurrentPatrolStep = -1;

    this->DebugInfo = TEXT("base");
}

CBaseBehavior::~CBaseBehavior() {}

FAIParentActionTickResult CBaseBehavior::AIParentActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    bool ShouldEnergyHeal = (
        Owner->UnitGetEnergy() < 0.5f &&
        Owner->EnergyHealItem != nullptr &&
        Owner->AIState.FindOrAdd(STATE_E_HEAL, 0) == 0
    );
    if (ShouldEnergyHeal) {
        Owner->AIState.Emplace(STATE_E_HEAL, 1);

        TArray<IAIActionExecutor*> Parts;
        Parts.Push(new CFindItemAction(RawOwner, Owner->EnergyHealItem, Owner->DefaultWorldSearchRadius, true));
        Parts.Push(new CUseItemAction(nullptr, nullptr));
        Parts.Push(new CUpdateStateAction(STATE_E_HEAL, 0));

        return FAIParentActionTickResult(false, new CMultiAction(Parts));
    }
    bool ShouldKineticHeal = (
        Owner->UnitGetKineticHealth() < 0.5f &&
        Owner->KineticHealItem != nullptr &&
        Owner->AIState.FindOrAdd(STATE_K_HEAL, 0) == 0
    );
    if (ShouldKineticHeal) {
        Owner->AIState.Emplace(STATE_K_HEAL, 1);

        TArray<IAIActionExecutor*> Parts;
        Parts.Push(new CFindItemAction(RawOwner, Owner->KineticHealItem, Owner->DefaultWorldSearchRadius, true));
        Parts.Push(new CUseItemAction(nullptr, nullptr));
        Parts.Push(new CUpdateStateAction(STATE_K_HEAL, 0));

        return FAIParentActionTickResult(false, new CMultiAction(Parts));
    }

    return FAIParentActionTickResult(false, nullptr);
}

FAIActionTickResult CBaseBehavior::AIActionTick(AActor* Owner, float DeltaTime) {
    int StepCount = this->PatrolSteps->Num();
    if (StepCount == 0) return this->Unfinished;

    this->CurrentPatrolStep = (this->CurrentPatrolStep + 1) % StepCount;

    return FAIActionTickResult(false, new CPatrolStepAction((*this->PatrolSteps)[this->CurrentPatrolStep]));
}
