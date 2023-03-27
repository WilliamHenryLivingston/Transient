#include "PatrolAction.h"

CPatrolAction::CPatrolAction(TArray<FAIPatrolStep>* Steps) {
    this->Steps = Steps;
    this->CurrentStep = -1;
}

CPatrolAction::~CPatrolAction() {}

FAIActionTickResult CPatrolAction::AIActionTick(AActor* Owner, float DeltaTime) {
    int StepCount = this->Steps->Num();
    if (StepCount == 0) return this->Unfinished;

    this->CurrentStep = (this->CurrentStep + 1) % StepCount;

    return FAIActionTickResult(false, new CPatrolStepAction((*this->Steps)[this->CurrentStep]));
}
