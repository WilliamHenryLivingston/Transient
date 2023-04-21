// Copyright: R. Saxifrage, 2023. All rights reserved.

// Performs all actions related to a single step of a patrol.

#pragma once

#include "CoreMinimal.h"

#include "Transient/AI/AINavNode.h"

#include "Action.h"

class CPatrolStepAction : public CAction {
private:
    AAINavNode* Node;

    bool TravelStarted;
    bool EquipStarted;
    bool WaitStarted;
    bool UseStarted;
    bool InteractStarted;
    bool SubjugateStarted;

public:
    CPatrolStepAction(AAINavNode* Node);
    virtual ~CPatrolStepAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
