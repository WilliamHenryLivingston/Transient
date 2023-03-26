#pragma once

#include "CoreMinimal.h"

#include "../ItemActor.h"
#include "AIActionExecutor.h"

class CTakeCoverAction : public IAIActionExecutor {
public:
    CTakeCoverAction();
    virtual ~CTakeCoverAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
