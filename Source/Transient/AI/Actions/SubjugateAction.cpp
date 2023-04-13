#include "SubjugateAction.h"

#include "../AIUnit.h"
#include "MoveToPointAction.h"
#include "EngageAction.h"

CSubjugateAction::CSubjugateAction(AActor* InitTarget) {
    this->Target = InitTarget;
    this->TravelStarted = false;
    this->HurtTimer = 2.0f;

    FString Name = TEXT("<missing>");
    if (this->Target != nullptr) {
        Name = this->Target->GetName();
    }
    this->DebugInfo = FString::Printf(TEXT("subjugate %s"), *Name);
}

CSubjugateAction::~CSubjugateAction() {}

FAIActionTickResult CSubjugateAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (this->Target == nullptr || !IsValid(this->Target)) return this->Finished;

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (!this->TravelStarted) {
        this->TravelStarted = true;

        return FAIActionTickResult(false, new CMoveToPointAction(this->Target, 200.0f));
    }

    if (this->HurtTimer > 0.0f) {
        Owner->UnitFaceTowards(this->Target->GetActorLocation());
        Owner->UnitSetTriggerPulled(this->HurtTimer < 0.5f);

        this->HurtTimer -= DeltaTime;
        if (this->HurtTimer <= 0.0f) {
            Owner->UnitSetTriggerPulled(false);
            Owner->UnitReload();
        }

        return this->Unfinished;
    }

    return this->Finished;
}
