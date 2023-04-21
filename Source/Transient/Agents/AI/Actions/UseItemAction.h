// Copyright: R. Saxifrage, 2023. All rights reserved.

// Uses an item in the units inventory. If the item is a weapon, it is reloaded. Applies
// to the active item if none is specified.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "Action.h"

class CUseItemAction : public CAction {
private:
    AItemActor* Item;
    AActor* UseTarget;

    bool UseStarted;
    float SetupTimer;

public:
    CUseItemAction(AItemActor* Item, AActor* UseTarget);
    virtual ~CUseItemAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
