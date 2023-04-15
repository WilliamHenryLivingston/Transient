#include "InteractAction.h"

#include "../AIUnit.h"
#include "MoveToPointAction.h"

CInteractAction::CInteractAction(AInteractiveActor* InitTarget) {
    this->Target = InitTarget;
    this->TravelStarted = false;
    this->InteractStarted = false;

    this->DebugInfo = FString::Printf(TEXT("interact %s"), *this->Target->GetName());
}

CInteractAction::~CInteractAction() {}

FAIActionTickResult CInteractAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (!this->Target->InteractEnabled) return this->Finished;

    if (!this->TravelStarted) {
        this->TravelStarted = true;

        return FAIActionTickResult(false, new CMoveToPointAction(this->Target, 50.0f));
    }

    if (!this->InteractStarted) {
        this->InteractStarted = true;

        Owner->UnitInteractWith(this->Target);
        return this->Unfinished;
    }

    return this->Finished;
}
