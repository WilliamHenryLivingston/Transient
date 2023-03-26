#pragma once

#include "CoreMinimal.h"

#include "../ItemActor.h"
#include "AIActionExecutor.h"

class CUseItemAction : public IAIActionExecutor {
private:
    AItemActor* Target;

    bool UseStarted;

public:
    CUseItemAction(AItemActor* Target);
    virtual ~CUseItemAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
