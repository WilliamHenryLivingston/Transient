// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "Action.h"

#include "Transient/Agents/Units/AIUnit.h"

FActionTickResult::FActionTickResult(bool InitFinished, CAction* InitPushChild) {
    this->Finished = InitFinished;
    this->PushChild = InitPushChild;
}

FActionParentTickResult::FActionParentTickResult(bool InitStopChildren, CAction* InitPushChild) {
    this->StopChildren = InitStopChildren;
    this->PushChild = InitPushChild;
}

CAction::~CAction() {}

FActionTickResult CAction::ActionTick(AUnitAgent* Owner, float DeltaTime) {
    return FActionTickResult(true, nullptr);
}

FActionParentTickResult CAction::ActionParentTick(AActor* Owner, float DeltaTime) {
    return FActionParentTickResult(false, nullptr);
}
