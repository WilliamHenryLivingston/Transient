#include "AIUnit.h"

void AAIUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FollowTarget != nullptr)
    {
        FVector FollowLocation = FollowTarget->GetActorLocation();

        UnitFaceTowards(FollowLocation);

        float Distance = (FollowLocation - GetActorLocation()).Length();
        if (Distance > 200.0f)
        {
            UnitMoveTowards(FollowLocation, DeltaTime);
        }
    }
}
