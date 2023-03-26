#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"

class CAttackAIAction : public IAIActionExecutor {
private:
    AActor* Target;

	float AttackTime;

public:
    CAttackAIAction(AActor* Target);
    virtual ~CAttackAIAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
