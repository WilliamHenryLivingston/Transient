// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CMultiAction : public IAIActionExecutor {
private:
    TArray<IAIActionExecutor*> Parts;

    int PartIndex;

public:
    CMultiAction(TArray<IAIActionExecutor*> Parts);
    virtual ~CMultiAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
