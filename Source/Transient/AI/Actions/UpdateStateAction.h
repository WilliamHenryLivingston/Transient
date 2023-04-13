// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"
#include "AIState.h"

class CUpdateStateAction : public IAIActionExecutor {
private:
    AI_STATE_T Key;
    int Value;

public:
    CUpdateStateAction(AI_STATE_T Key, int Value);
    virtual ~CUpdateStateAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
