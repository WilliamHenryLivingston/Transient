// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CFindItemAction : public IAIActionExecutor {
private:
    AItemActor* Target;

    bool TravelStarted;

public:
    static CFindItemAction* AIActionTryPlan(AActor* Owner, TSubclassOf<AItemActor> TargetType, float MaxDistance);
    virtual ~CFindItemAction() override;

public:
    CFindItemAction(AItemActor* Target);
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
