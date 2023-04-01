// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIActionExecutor.h"
#include "PatrolStepAction.h"

class CMoveToPointAction : public IAIActionExecutor {
private:
    AActor* Target;
    float ReachDistance;

    bool Planned;
    TArray<FVector> Steps;

public:
    CMoveToPointAction(AActor* Target, float ReachDistance);
    virtual ~CMoveToPointAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;

private:
    void PlanMove(AActor* Owner);
};
