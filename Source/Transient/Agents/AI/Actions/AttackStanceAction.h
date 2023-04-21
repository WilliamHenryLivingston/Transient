// Copyright: R. Saxifrage, 2023. All rights reserved.

// Hold an attack stance for a period of time, managing facing.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "Action.h"

class CAttackStanceAction : public CAction {
private:
    AActor* Target;

    float Timer;
    bool PopOut;
    float PopOutTimer;

public:
    CAttackStanceAction(AActor* Target, float Duration, bool PopOut);
    virtual ~CAttackStanceAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
