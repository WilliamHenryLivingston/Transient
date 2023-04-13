#include "EngageAction.h"

#include "../AIUnit.h"

CEngageAction::CEngageAction(AActor* InitTarget, float Duration, bool InitPopOut) {
    this->Timer = Duration;
    this->Target = InitTarget;
    this->PopOut = InitPopOut;
    this->PopOutTimer = 0.0f;

    this->DebugInfo = FString::Printf(TEXT("engage %s for %.2f"), *this->Target->GetName(), this->Timer);
}

CEngageAction::~CEngageAction() {}

FAIActionTickResult CEngageAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (this->Timer > 0.0f) {
        this->Timer -= DeltaTime;

        if (this->PopOut && !Owner->UnitAreArmsOccupied()) {
            if (this->PopOutTimer > 0.0f) {
                this->PopOutTimer -= DeltaTime;
                Owner->UnitSetTriggerPulled(true);
            }
            else {
                this->PopOutTimer = FMath::RandRange(0.5f, 2.0f);

                Owner->UnitSetCrouched(!Owner->UnitIsCrouched());
            }
        }
        else {
            Owner->UnitSetCrouched(true);
        }

        Owner->UnitFaceTowards(this->Target->GetActorLocation());

        return this->Unfinished;
    }

    return this->Finished;
}
