#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"
#include "PatrolStepAIAction.h"

class CMoveToPointAction : public IAIActionExecutor {
private:
    AActor* Target;
    float ReachDistance;

    bool Planned;
    TArray<AActor*> Steps;

public:
    CMoveToPointAction(AActor* Target, float ReachDistance);
    virtual ~CMoveToPointAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;

private:
    void PlanMove(AActor* Owner);
};
