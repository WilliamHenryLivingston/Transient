// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CUpdateStateAction : public IAIActionExecutor {
private:
    FString Key;
    int Value;

public:
    CUpdateStateAction(FString Key, int Value);
    virtual ~CUpdateStateAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
