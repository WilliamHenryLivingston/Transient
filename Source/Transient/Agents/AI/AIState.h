// Copyright: R. Saxifrage, 2023. All rights reserved.

// AI state supports action context-awareness and is represented as a map.

#pragma once

#include "CoreMinimal.h"

#define SK_ENERGY_HEAL 1
#define SK_KINETIC_HEAL 2
#define SK_ATTACK_ACTIVE 3
#define SK_IN_COVER 4

typedef TAIStateKey int;
typedef TAIStateVal int;

class CAIState {
private:
    TMap<TAIStateKey, TAIStateVal> StateMap;

public:
    CAIState();
    TAIStateVal StateRead(TAIStateKey Key) const;
    void StateWrite(TAIStateKey Key, TAIStateVal Val);
};
