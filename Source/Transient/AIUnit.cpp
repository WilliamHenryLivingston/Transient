#include "AIUnit.h"

void AAIUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FollowTarget != nullptr)
    {
        FVector FollowLocation = FollowTarget->GetActorLocation();

        UnitFaceTowards(FollowLocation);

        float Distance = (FollowLocation - GetActorLocation()).Length();
        if (Distance > 300.0f)
        {
            UnitMoveTowards(FollowLocation, DeltaTime);
        }

        UnitSetFiring(Distance < 600.0f);
    }
}
