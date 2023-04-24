// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "MoveToPointAction.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "Transient/Agents/AgentManager.h"
#include "Transient/Agents/AI/AINavNode.h"

CMoveToPointAction::CMoveToPointAction(FVector InitTarget, float InitReachDistance, bool InitIntoCover) {
    this->Target = InitTarget;
    this->ReachDistance = InitReachDistance;
    this->IntoCover = InitIntoCover;

    this->Planned = false;
    this->Steps = TArray<FVector>();

#if DEBUG_AI
    this->DebugInfo = FString::Printf(TEXT("moveto %s"), *this->Target->GetName());
#endif
}

CMoveToPointAction::~CMoveToPointAction() {}

FActionTickResult CMoveToPointAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (!this->Planned) {
        this->ActionPlanMove(Owner);
        if (this->Steps.Num() == 0) return FActionTickResult::Error(0);

        this->Planned = true;
    }

    Owner->UnitSetLegState(EUnitLegState::Normal);
    State->StateWrite(SK_IN_COVER, 0);

    FVector NextLocation = this->Steps[0];
    Owner->UnitMoveTowards(NextLocation);
    if (AIState->StateRead(SK_ATTACK_ACTIVE) == 0) {
        Owner->UnitFaceTowards(NextLocation);
    }

    float Distance = (NextLocation - OwnerLocation).Size2D();
    if (Distance > this->ReachDistance) return FActionTickResult::Unfinished;
    
    this->Steps.RemoveAt(0);
    if (this->Steps.Num() > 1) return FActionTickResult::Unfinished;

    State->StateWrite(SK_IN_COVER, this->IntoCover ? 1 : 0);
    return FActionTickResult::Finished;
}

void CMoveToPointAction::ActionPlanMove(AUnitAgent* Owner) {
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Owner->GetWorld());
    AAgentManager* Manager = AAgentManager::AgentsGetManager(Owner->GetWorld());

    FPathFindingQuery Query = FPathFindingQuery(
        Owner,
        *Manager->NavData,
        Owner->GetActorLocation(),
        this->Target,
        FSharedConstNavQueryFilter(),
        FNavPathSharedPtr()
    );
    FPathFindingResult Result = NavSystem->FindPathSync(Query, EPathFindingMode::Type::Regular);

    TArray<FNavPathPoint> PathPoints = Result.Path->GetPathPoints();
    for (int i = 0; i < PathPoints.Num(); i++) {
        this->Steps.Push(PathPoints[i]);
    }
}
