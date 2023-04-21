// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InteractAction.h"

#include "MoveToPointAction.h"

#define INTERACT_DIST 50.0f

CInteractAction::CInteractAction(AInteractiveAgent* InitTarget) {
    this->Target = InitTarget;

    this->TravelStarted = false;
    this->InteractStarted = false;

#if DEBUG_ACTIONS
    this->DebugInfo = FString::Printf(TEXT("interact %s"), *this->Target->GetName());
#endif
}

CInteractAction::~CInteractAction() {}

FActionTickResult CInteractAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (Owner->UnitArmsOccupied()) return FActionTickResult::Unfinished;

    if (!this->Target->InteractEnabled) return FActionTickResult::Error(0);

    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FActionTickResult::UnfinishedAnd(new CMoveToPointAction(this->Target, INTERACT_DIST, false));
    }

    if (!this->InteractStarted) {
        this->InteractStarted = true;

        Owner->UnitInteractWith(this->Target);
        return FActionTickResult::Unfinished;
    }

    return FActionTickResult::Finished;
}
