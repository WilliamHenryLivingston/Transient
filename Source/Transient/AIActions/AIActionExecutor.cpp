#include "AIActionExecutor.h"

#include "../AIUnit.h"

FAIActionExecutionResult::FAIActionExecutionResult() {
    this->Finished = false;
    this->PushInner = nullptr;
}

FAIActionExecutionResult::FAIActionExecutionResult(bool Finished, IAIActionExecutor* PushInner) {
    this->Finished = Finished;
    this->PushInner = PushInner;
}

IAIActionExecutor::IAIActionExecutor() {
    this->Unfinished = FAIActionExecutionResult(false, nullptr);
    this->Finished = FAIActionExecutionResult(true, nullptr);
}

IAIActionExecutor::~IAIActionExecutor() {}

FAIActionExecutionResult IAIActionExecutor::AIActionTick(AActor* Owner, float DeltaTime) {
    return FAIActionExecutionResult(true, nullptr);
}
