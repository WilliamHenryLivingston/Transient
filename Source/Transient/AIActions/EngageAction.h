// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../ItemActor.h"
#include "AIActionExecutor.h"

class CEngageAction : public IAIActionExecutor {
private:
    AActor* Target;

    float Timer;
    bool PopOut;
    float PopOutTimer;

public:
    CEngageAction(AActor* Target, float Duration, bool PopOut);
    virtual ~CEngageAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
