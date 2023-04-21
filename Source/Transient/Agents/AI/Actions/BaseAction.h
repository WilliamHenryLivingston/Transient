// Copyright: R. Saxifrage, 2023. All rights reserved.

// The base action controlling a units behavior.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"
#include "PatrolStepAction.h"

class CBaseAction : public CAction {
private:
    TArray<AAINavNode*>* PatrolSteps;

    int CurrentPatrolStep;

public:
    CBaseAction(TArray<AAINavNode*>* PatrolSteps);
    virtual ~CBaseAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
    virtual FActionParentTickResult ActionParentTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
