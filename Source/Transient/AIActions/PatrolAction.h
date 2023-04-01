// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"
#include "PatrolStepAction.h"

class CPatrolAction : public IAIActionExecutor {
private:
    TArray<FAIPatrolStep>* Steps;

    int CurrentStep;

public:
    CPatrolAction(TArray<FAIPatrolStep>* Steps);
    virtual ~CPatrolAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
