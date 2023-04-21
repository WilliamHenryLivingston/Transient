// Copyright: R. Saxifrage, 2023. All rights reserved.

// The base action controlling a units behavior when it has attack targets.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/AI/AINavNode.h"

#include "Action.h"

class CAttackAction : public CAction {

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
	virtual FActionParentTickResult ActionParentTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;

private:
    AAgentActor* ActionCurrentTarget(AUnitAgent* Owner);
    CAction* ActionHandleWeapon(AUnitAgent* Owner);
    void ActionHandleTriggerAndFacing(AUnitAgent* Owner);
};
