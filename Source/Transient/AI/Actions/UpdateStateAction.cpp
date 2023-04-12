#include "UpdateStateAction.h"

#include "../AIUnit.h"

CUpdateStateAction::CUpdateStateAction(FString InitKey, int InitValue) {
    this->Key = InitKey;
    this->Value = InitValue;

    this->DebugInfo = FString::Printf(TEXT("stateup %s=%d"), *this->Key, this->Value);
}

CUpdateStateAction::~CUpdateStateAction() {}

FAIActionTickResult CUpdateStateAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    Owner->AIState.Emplace(this->Key, this->Value);

    return this->Finished;
}
