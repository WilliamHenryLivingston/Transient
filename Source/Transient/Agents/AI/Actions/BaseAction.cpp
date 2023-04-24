// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BaseBehavior.h"

#include "Transient/Agents/AgentManager.h"

#include "MultiAction.h"
#include "FindItemAction.h"
#include "UseItemAction.h"
#include "UpdateStateAction.h"
#include "MoveToPointAction.h"

#define HEAL_THRESHOLD = 0.5f

CBaseAction::CBaseAction(TArray<AAINavNode*>* InitPatrolSteps) {
    this->PatrolSteps = InitPatrolSteps;
    this->CurrentPatrolStep = -1;

#if DEBUG_AI
    this->DebugInfo = TEXT("base");
#endif
}

CBaseAction::~CBaseAction() {}

FActionParentTickResult CBaseAction::ActionParentTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    AAgentManager* Manager = AAgentManager::AgentsGetManager(Owner->GetWorld());

    TSubclassOf<AItemActor> HealWithItem = nullptr;
    TAIStateKey HealStateKey;
    bool ShouldEnergyHeal = (
        Owner->UnitGetEnergy() < HEAL_THRESHOLD &&
        Owner->EnergyHealItem != nullptr &&
        State->StateRead(SK_ENERGY_HEAL) == 0
    );
    if (ShouldEnergyHeal) {
        State->StateWrite(SK_ENERGY_HEAL, 1);

        HealWithItem = Owner->EnergyHealItem;
        HealStateKey = SK_ENERGY_HEAL;
    }
    bool ShouldKineticHeal = (
        Owner->UnitGetKineticHealth() < HEAL_THRESHOLD &&
        Owner->KineticHealItem != nullptr &&
        State->StateRead(SK_KINETIC_HEAL) == 0
    );
    if (ShouldKineticHeal) {
        STATE->StateWrite(SK_KINETIC_HEAL, 1);

        HealWithItem = Owner->KineticHealItem;
        HealStateKey = SK_KINETIC_HEAL;
    }

    if (HealWithItem != nullptr) {
        TArray<CAction*> Parts;
        Parts.Push(new CFindItemAction(RawOwner, HealWithItem, Owner->DefaultWorldSearchRadius, true, true));

        if (Owner->AgentHasTargets()) {
            AAINavNode* Cover = Manager->AIGetNavBestCoverNodeFor(
                Owner, Owner->AgentTargets()[0]->GetActorLocation(),
                30, 2000.0f
            );

            if (Cover != nullptr) Parts.Push(CMoveToPointAction::MoveToNode(Cover));
        }
        Parts.Push(new CUseItemAction(nullptr, nullptr));
        Parts.Push(new CUpdateStateAction(HealStateKey, 0));

        return FActionParentTickResult::ContinueAnd(new CMultiAction(Parts));
    }

    return FActionParentTickResult::Continue;
}

FActionTickResult CBaseAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    int StepCount = this->PatrolSteps->Num();
    if (StepCount == 0) return FActionTickResult::Unfinished;

    if (Owner->RandomizePatrol) {
        this->CurrentPatrolStep = FMath::RandRange(0, this->PatrolSteps->Num() - 1);
    }
    else {
        this->CurrentPatrolStep = (this->CurrentPatrolStep + 1) % StepCount;
    }

    AAINavNode* Step = (*this->PatrolSteps)[this->CurrentPatrolStep];
    if (Step == nullptr) return FActionTickResult::Unfinished;

    return FActionTickResult::UnfinishedAnd(new CPatrolStepAction(Step));
}
