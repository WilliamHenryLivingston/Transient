// Copyright: R. Saxifrage, 2023. All rights reserved.

// Executes a set of actions in order.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"

class CMultiAction : public CAction {
private:
    TArray<CAction*> Parts;

    int PartIndex;

public:
    CMultiAction(TArray<CAction*> Parts);
    virtual ~CMultiAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
