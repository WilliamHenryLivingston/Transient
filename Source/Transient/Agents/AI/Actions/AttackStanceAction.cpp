// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AttackStanceAction.h"

CAttackStanceAction::CAttackStanceAction(AActor* InitTarget, float Duration, bool InitPopOut) {
    this->Timer = Duration;
    this->Target = InitTarget;
    this->PopOut = InitPopOut;

    this->PopOutTimer = 0.0f;

#if DEBUG_AI
    this->DebugInfo = FString::Printf(TEXT("attackstance %s for %.2f"), *this->Target->GetName(), this->Timer);
#endif
}

CAttackStanceAction::~CAttackStanceAction() {}

FActionTickResult CAttackStanceAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (this->Timer > 0.0f) {
        this->Timer -= DeltaTime;

        if (this->PopOut && !Owner->UnitArmsOccupied()) {
            if (this->PopOutTimer > 0.0f) {
                this->PopOutTimer -= DeltaTime;
                Owner->UnitSetTriggerPulled(true);
            }
            else {
                this->PopOutTimer = FMath::RandRange(0.5f, 2.0f);

                Owner->UnitSetLegState(
                    Owner->UnitLegState() == EUnitLegState::Normal ?
                        EUnitLegState::Crouch : EUnitLegState::Normal
                );
            }
        }
        else {
            Owner->UnitSetLegState(EUnitLegState::Normal);
        }

        Owner->UnitFaceTowards(this->Target->GetActorLocation());

        return FActionTickResult::Unfinished;
    }

    return FActionTickResult::Finished;
}
