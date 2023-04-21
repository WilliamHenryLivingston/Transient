// Copyright: R. Saxifrage, 2023. All rights reserved.

// Updates a single entry in AI state. Useful when composing multi-actions.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"

class CUpdateStateAction : public CAction {
private:
    TAIStateKey Key;
    TAIStateVal Value;

public:
    CUpdateStateAction(TAIStateKey Key, TAIStateVal Value);
    virtual ~CUpdateStateAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
