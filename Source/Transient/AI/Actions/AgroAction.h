// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../AINavNode.h"
#include "AIActionExecutor.h"

class CAgroAction : public IAIActionExecutor {
private:
    AActor* Target;

public:
    CAgroAction(AActor* Target);
    virtual ~CAgroAction() override;

public:
	virtual AActor* AIActionAgroTarget() override;
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
	virtual FAIParentActionTickResult AIParentActionTick(AActor* Owner, float DeltaTime) override;

private:
    IAIActionExecutor* HandleWeapon(AActor* Owner);
    bool HandleTriggerAndFacing(AActor* Owner);
};
