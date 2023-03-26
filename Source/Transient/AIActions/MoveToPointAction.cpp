#include "MoveToPointAction.h"

#include "../AIUnit.h"

CMoveToPointAction::CMoveToPointAction(AActor* InitTarget, float InitReachDistance) {
    this->Target = InitTarget;
    this->ReachDistance = InitReachDistance;

    this->Planned = false;
    this->Steps = TArray<AActor*>();
}

CMoveToPointAction::~CMoveToPointAction() {}

FAIActionExecutionResult CMoveToPointAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    if (this->Target == nullptr || !IsValid(this->Target)) return this->Finished;

    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->Planned) {
        this->Steps.Push(this->Target);
        this->PlanMove(Owner);
        this->Planned = true;
    }

    if (this->Steps.Num() > 0) {
        FVector TargetLocation = this->Steps[0]->GetActorLocation();

        Owner->UnitMoveTowards(TargetLocation);
        Owner->UnitFaceTowards(TargetLocation);

        bool Reached = (TargetLocation - Owner->GetActorLocation()).Size() < this->ReachDistance;
        if (Reached) this->Steps.RemoveAt(0);

        return this->Unfinished;
    }

    return this->Finished;
}

void CMoveToPointAction::PlanMove(AActor* Owner) {
    // TODO.
}