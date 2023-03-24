#include "PatrolStepAIAction.h"

#include "../AIUnit.h"

CPatrolStepAIAction::CPatrolStepAIAction(FAIPatrolStep Step) {
    this->Step = Step;
}

CPatrolStepAIAction::~CPatrolStepAIAction() {}

bool CPatrolStepAIAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->TravelFinished) {
        FVector TargetLocation = this->Step.Node->GetActorLocation();

        Owner->UnitMoveTowards(TargetLocation);
        Owner->UnitFaceTowards(TargetLocation);

        this->TravelFinished = (TargetLocation - Owner->GetActorLocation()).Size() < this->Step.NodeReachDistance;
        return false;
    }

    if (!this->AnimationFinished) {
        // TODO.
        this->AnimationFinished = true;
        this->Timer = FMath::RandRange(this->Step.WaitTimeMin, this->Step.WaitTimeMax);
        return false;
    }

    if (this->Timer > 0.0) {
        this->Timer -= DeltaTime;
        return false;
    }

    return true;
}
