#include "MultiAction.h"

#include "../AIUnit.h"

CMultiAction::CMultiAction(TArray<IAIActionExecutor*> InitParts) {
    this->Parts = InitParts;
    this->PartIndex = 0;

    this->DebugInfo = FString::Printf(TEXT("multi %d"), this->Parts.Num());
}

CMultiAction::~CMultiAction() {}

FAIActionTickResult CMultiAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (this->PartIndex < this->Parts.Num()) {
        return FAIActionTickResult(false, this->Parts[this->PartIndex++]);
    }

    return this->Finished;
}
