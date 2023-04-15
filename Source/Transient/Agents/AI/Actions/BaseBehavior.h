// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"
#include "PatrolStepAction.h"

class CBaseBehavior : public IAIActionExecutor {
private:
    TArray<AAINavNode*>* PatrolSteps;

    int CurrentPatrolStep;

public:
    CBaseBehavior(TArray<AAINavNode*>* PatrolSteps);
    virtual ~CBaseBehavior() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
    virtual FAIParentActionTickResult AIParentActionTick(AActor* Owner, float DeltaTime) override;
};
