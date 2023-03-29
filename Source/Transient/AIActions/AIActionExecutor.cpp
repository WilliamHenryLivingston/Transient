#include "AIActionExecutor.h"

#include "../AIUnit.h"

FAIActionTickResult::FAIActionTickResult() {
    this->Finished = false;
    this->PushChild = nullptr;
}

FAIActionTickResult::FAIActionTickResult(bool InitFinished, IAIActionExecutor* InitPushChild) {
    this->Finished = InitFinished;
    this->PushChild = InitPushChild;
}

FAIParentActionTickResult::FAIParentActionTickResult() {
    this->StopChildren = false;
    this->PushChild = nullptr;
}

FAIParentActionTickResult::FAIParentActionTickResult(bool InitStopChildren, IAIActionExecutor* InitPushChild) {
    this->StopChildren = InitStopChildren;
    this->PushChild = InitPushChild;
}

IAIActionExecutor::IAIActionExecutor() {
    this->Unfinished = FAIActionTickResult(false, nullptr);
    this->Finished = FAIActionTickResult(true, nullptr);
}

IAIActionExecutor::~IAIActionExecutor() {}

bool IAIActionExecutor::AIActionIsAttackOn(AActor* Target) {
    return false;
}

FAIActionTickResult IAIActionExecutor::AIActionTick(AActor* Owner, float DeltaTime) {
    return FAIActionTickResult(true, nullptr);
}

FAIParentActionTickResult IAIActionExecutor::AIParentActionTick(AActor* Owner, float DeltaTime) {
    return FAIParentActionTickResult(false, nullptr);
}
