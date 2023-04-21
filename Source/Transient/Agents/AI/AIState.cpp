// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIState.h"

CAIState::CAIState() {
    this->StateMap = TMap<TAIStateKey, TAIStateVal>();
}

TAIStateVal CAIState::StateRead(TAIStateKey Key) const {
    return this->StateMap.ReadOrAdd(Key, 0);
}

void CAIState::StateWrite(TAIStateKey Key, TAIStateVal Val) {
    this->StateMap.Emplace(Key, Val);
}
