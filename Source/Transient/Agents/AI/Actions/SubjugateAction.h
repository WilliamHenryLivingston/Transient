// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CSubjugateAction : public IAIActionExecutor {
private:
    AActor* Target;

    bool TravelStarted;
    float HurtTimer;

public:
    CSubjugateAction(AActor* Target);
    virtual ~CSubjugateAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
