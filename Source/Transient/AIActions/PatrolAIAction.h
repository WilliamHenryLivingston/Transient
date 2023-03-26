#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"
#include "PatrolStepAIAction.h"

class CPatrolAIAction : public IAIActionExecutor {
private:
    TArray<FAIPatrolStep>* Steps;

    int CurrentStep;

public:
    CPatrolAIAction(TArray<FAIPatrolStep>* Steps);
    virtual ~CPatrolAIAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
