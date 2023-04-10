// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../../Items/ItemActor.h"
#include "AIActionExecutor.h"

class CWaitAction : public IAIActionExecutor {
private:
    float Timer;
	float ScanTimer;

    bool ScanIfIdle;

	FVector ScanVector;

public:
    CWaitAction(float Duration, bool ScanIfIdle);
    virtual ~CWaitAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
