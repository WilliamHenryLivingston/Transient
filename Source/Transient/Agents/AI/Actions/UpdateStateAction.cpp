// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "UpdateStateAction.h"

CUpdateStateAction::CUpdateStateAction(TAIStateKey InitKey, TAIStateVal InitValue) {
    this->Key = InitKey;
    this->Value = InitValue;

#if DEBUG_AI
    this->DebugInfo = FString::Printf(TEXT("state update %d=%d"), this->Key, this->Value);
#endif
}

CUpdateStateAction::~CUpdateStateAction() {}

FActionTickResult CUpdateStateAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    State->StateWrite(this->Key, this->Value);

    return FActionTickResult::Finished;
}
