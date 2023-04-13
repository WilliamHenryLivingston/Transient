// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "../AINavNode.h"
#include "AIActionExecutor.h"

class CPatrolStepAction : public IAIActionExecutor {
private:
    AAINavNode* Node;

    bool TravelStarted;
    bool EquipStarted;
    bool WaitStarted;
    bool UseStarted;
    bool SubjugateStarted;

public:
    CPatrolStepAction(AAINavNode* Node);
    virtual ~CPatrolStepAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
