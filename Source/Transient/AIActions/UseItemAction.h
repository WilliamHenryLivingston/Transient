#pragma once

#include "CoreMinimal.h"

#include "../ItemActor.h"
#include "AIActionExecutor.h"

class CUseItemAction : public IAIActionExecutor {
private:
    AItemActor* Target;
    AActor* UseTarget;

    bool UseStarted;
    float Timer;

public:
    CUseItemAction(AItemActor* Target, AActor* UseTarget);
    virtual ~CUseItemAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
