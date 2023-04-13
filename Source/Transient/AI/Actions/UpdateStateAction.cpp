#include "UpdateStateAction.h"

#include "../AIUnit.h"

CUpdateStateAction::CUpdateStateAction(AI_STATE_T InitKey, int InitValue) {
    this->Key = InitKey;
    this->Value = InitValue;

    this->DebugInfo = FString::Printf(TEXT("stateup %d=%d"), this->Key, this->Value);
}

CUpdateStateAction::~CUpdateStateAction() {}

FAIActionTickResult CUpdateStateAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    Owner->AIState.Emplace(this->Key, this->Value);

    return this->Finished;
}
