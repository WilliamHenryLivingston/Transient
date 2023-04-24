// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "MultiAction.h"

CMultiAction::CMultiAction(TArray<CAction*> InitParts) {
    this->Parts = InitParts;
    this->PartIndex = 0;

#if DEBUG_AI
    this->DebugInfo = FString::Printf(TEXT("multi %d"), this->Parts.Num());
#endif
}

CMultiAction::~CMultiAction() {
    // TODO: Memory leak; delete all unused parts.
}

FActionTickResult CMultiAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (this->PartIndex < this->Parts.Num()) {
        return FActionTickResult::UnfinishedAnd(this->Parts[this->PartIndex++]);
    }

    return FActionTickResult::Finished;
}
