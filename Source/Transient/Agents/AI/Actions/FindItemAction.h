// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CFindItemAction : public IAIActionExecutor {
public:
    AItemActor* Target;

private:
    TSubclassOf<AItemActor> TargetType;

    bool Started;
    bool TakeStarted;
    bool EquipStarted;

public:
    CFindItemAction(AActor* Owner, TSubclassOf<AItemActor> TargetType, float MaxDistance, bool EquipAfter);
    virtual ~CFindItemAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
