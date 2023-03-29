#pragma once

#include "CoreMinimal.h"

#include "../ItemActor.h"
#include "AIActionExecutor.h"

class CEquipItemAction : public IAIActionExecutor {
private:
    AItemActor* Target;

    bool Started;

public:
    CEquipItemAction(AItemActor* Target);
    virtual ~CEquipItemAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
