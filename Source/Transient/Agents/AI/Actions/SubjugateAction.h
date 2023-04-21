// Copyright: R. Saxifrage, 2023. All rights reserved.

// Briefly attacks the given target.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"

class CSubjugateAction : public CAction {
private:
    AAgentActor* Target;

    bool TravelStarted;
    float HurtTimer;

public:
    CSubjugateAction(AAgentActor* Target);
    virtual ~CSubjugateAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
