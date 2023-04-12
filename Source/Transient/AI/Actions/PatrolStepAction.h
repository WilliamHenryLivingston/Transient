// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "../AINavNode.h"
#include "AIActionExecutor.h"

#include "PatrolStepAction.generated.h"

class CPatrolStepAction : public IAIActionExecutor {
private:
    FAIPatrolStep Step;

    bool TravelStarted;
    bool UseFinished;

	int RemainingItemUseTimes;

public:
    CPatrolStepAction(FAIPatrolStep Step);
    virtual ~CPatrolStepAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
