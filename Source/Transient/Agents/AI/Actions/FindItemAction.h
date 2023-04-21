// Copyright: R. Saxifrage, 2023. All rights reserved.

// Finds and takes an item in the world, optionally spawning it if it wasn't found.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "Action.h"

class CFindItemAction : public CAction {
private:
    TSubclassOf<AItemActor> TargetType;

    AItemActor* Target;

    float MaxDistance;
    bool AllowSpawn;

    bool FindAttempted;

    bool Started;
    bool TakeStarted;
    bool EquipStarted;

public:
    static CFindItemAction* FindInWorldOrFail(AUnitAgent* Owner, TSubclassOf<AItemActor> TargetType, float MaxDistance, bool EquipAfter);
    CFindItemAction(TSubclassOf<AItemActor> TargetType, float MaxDistance, bool AllowSpawn, bool EquipAfter);
    virtual ~CFindItemAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;

private:
    AItemActor* FindTargetInWorld(AUnitAgent* Owner);
};
