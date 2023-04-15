// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Environment/InteractiveActor.h"
#include "AIActionExecutor.h"

class CInteractAction : public IAIActionExecutor {
private:
    AInteractiveActor* Target;

    bool TravelStarted;
    bool InteractStarted;

public:
    CInteractAction(AInteractiveActor* Target);
    virtual ~CInteractAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
