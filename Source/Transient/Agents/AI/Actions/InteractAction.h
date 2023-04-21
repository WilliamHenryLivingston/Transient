// Copyright: R. Saxifrage, 2023. All rights reserved.

// Moves to and interacts with the given target.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/InteractiveAgent.h"
#include "Action.h"

class CInteractAction : public CAction {
private:
    AInteractiveAgent* Target;

    bool TravelStarted;
    bool InteractStarted;

public:
    CInteractAction(AInteractiveAgent* Target);
    virtual ~CInteractAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
