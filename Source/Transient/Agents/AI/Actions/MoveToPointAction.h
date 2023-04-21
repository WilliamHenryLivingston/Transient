// Copyright: R. Saxifrage, 2023. All rights reserved.

// Moves the unit to the given target point with pathfinding.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"

class CMoveToPointAction : public CAction {
private:
    FVector Target;
    float ReachDistance;
    bool IntoCover;

    bool Planned;
    TArray<FVector> Steps;

public:
    CMoveToPointAction(FVector Target, float ReachDistance, bool IntoCover);
    virtual ~CMoveToPointAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;

private:
    void ActionPlanMove(AUnitAgent* Owner);
};
