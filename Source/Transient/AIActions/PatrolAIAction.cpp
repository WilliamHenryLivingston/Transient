#include "PatrolAIAction.h"

CPatrolAIAction::CPatrolAIAction(TArray<FAIPatrolStep>* Steps) {
    this->Steps = Steps;
    this->CurrentStep = 0;
}

CPatrolAIAction::~CPatrolAIAction() {}

FAIActionExecutionResult CPatrolAIAction::AIActionTick(AActor* Owner, float DeltaTime) {
    int StepCount = this->Steps->Num();
    if (StepCount == 0) return this->Unfinished;

    this->CurrentStep = (this->CurrentStep + 1) % StepCount;

    return FAIActionExecutionResult(false, new CPatrolStepAIAction((*this->Steps)[this->CurrentStep]));
}
