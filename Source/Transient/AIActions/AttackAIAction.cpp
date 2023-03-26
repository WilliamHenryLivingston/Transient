#include "AttackAIAction.h"

#include "../AIUnit.h"

CAttackAIAction::CAttackAIAction(AActor* InitTarget) {
    this->Target = InitTarget;
    this->AttackTime = 0.0f;
}

CAttackAIAction::~CAttackAIAction() {}

FAIActionExecutionResult CAttackAIAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    if (!IsValid(this->Target)) return this->Finished;

    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    AWeaponItem* CurrentWeapon = Owner->UnitGetActiveWeapon();
    if (CurrentWeapon != nullptr && CurrentWeapon->WeaponEmpty()) {
        Owner->UnitReload();
    }
    else {
        FVector MoveTowards = Target->GetActorLocation();

        float Distance = (MoveTowards - Owner->GetActorLocation()).Length();

        FVector Forward = Owner->GetActorForwardVector();
        FVector Move = MoveTowards - Owner->GetActorLocation();
        float Angle = acos(Move.Dot(Forward) / (Move.Length() * Forward.Length()));
        if (Angle > 0.15f) {
            Owner->UnitFaceTowards(MoveTowards);
        }

        if (this->AttackTime <= 0.0f && Distance > 300.0f) {
            Owner->UnitMoveTowards(MoveTowards);
        }
        else if (this->AttackTime <= 0.0f && Distance < 600.0f) {
            this->AttackTime = 2.0f;
        }

        Owner->UnitSetCrouched(this->AttackTime > 0.0f);
        bool IsCrouched = Owner->UnitIsCrouched();
        Owner->UnitSetTriggerPulled(IsCrouched);
        if (IsCrouched) {
            this->AttackTime -= DeltaTime;
        }
    }

    return this->Unfinished;
}
