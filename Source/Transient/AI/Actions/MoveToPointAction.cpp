#include "MoveToPointAction.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "../../Debug.h"
#include "../AIManager.h"
#include "../AIUnit.h"

// TODO: Take vector as target.
CMoveToPointAction::CMoveToPointAction(AActor* InitTarget, float InitReachDistance) {
    this->Target = InitTarget;
    this->ReachDistance = InitReachDistance;

    this->Planned = false;
    this->Steps = TArray<FVector>();
}

CMoveToPointAction::~CMoveToPointAction() {}

FAIActionTickResult CMoveToPointAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    if (this->Target == nullptr || !IsValid(this->Target)) return this->Finished;

    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    Owner->UnitUpdateTorsoPitch(0.0f);

    if (!this->Planned) {
        this->PlanMove(Owner);
        this->Planned = true;
    }
    
    Owner->UnitSetCrouched(false);

    if (this->Steps.Num() == 0) {
        ERR_LOG(Owner, "pathfind fail");
        return this->Finished;
    }

    FVector TargetLocation = this->Steps[0];

    Owner->UnitMoveTowards(TargetLocation);
    if (!Owner->UnitHasFaceTarget()) {
        Owner->UnitFaceTowards(TargetLocation);
    }

    FVector OwnerLocation = Owner->GetActorLocation();
    OwnerLocation.Z = 0;
    TargetLocation.Z = 0;

    bool Reached = (TargetLocation - OwnerLocation).Size() < this->ReachDistance;
    if (Reached) {
        if (this->Steps.Num() == 1) return this->Finished;

        this->Steps.RemoveAt(0);
    }

    return this->Unfinished;
}

void CMoveToPointAction::PlanMove(AActor* Owner) {
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Owner->GetWorld());
    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    FSharedConstNavQueryFilter Filter;
    FNavPathSharedPtr PathPtr;
    FPathFindingQuery Query(
        Owner,
        *Manager->NavData,
        Owner->GetActorLocation(),
        this->Target->GetActorLocation(),
        Filter,
        PathPtr
    );
    FPathFindingResult Result = NavSys->FindPathSync(Query, EPathFindingMode::Type::Regular);

    TArray<FNavPathPoint> PathPoints = Result.Path->GetPathPoints();
    for (int i = 0; i < PathPoints.Num(); i++) {
        this->Steps.Push(PathPoints[i]);
    }
}