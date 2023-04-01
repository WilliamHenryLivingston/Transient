// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../AINavNode.h"
#include "AIActionExecutor.h"

class CAttackAction : public IAIActionExecutor {
private:
    AActor* Target;
    AAINavNode* Cover;

	float AttackTime;
    bool Engaging;

public:
    CAttackAction(AActor* Target);
    virtual ~CAttackAction() override;

public:
	virtual bool AIActionIsAttackOn(AActor* Target) override;
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
	virtual FAIParentActionTickResult AIParentActionTick(AActor* Owner, float DeltaTime) override;

private:
    void HandleWeapon(AActor* Owner);
};
