// Copyright: R. Saxifrage, 2023. All rights reserved.

// Equips an item present in the units inventory.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "Action.h"

class CEquipItemAction : public CAction {
private:
    AItemActor* Target;

    bool Started;

public:
    CEquipItemAction(AItemActor* Target);
    virtual ~CEquipItemAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
