#include "AIUnit.h"

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->FollowTarget != nullptr) {
        FVector FollowLocation = this->FollowTarget->GetActorLocation();

        this->UnitFaceTowards(FollowLocation, DeltaTime);

        float Distance = (FollowLocation - this->GetActorLocation()).Length();
        if (Distance > 300.0f) {
            this->UnitMoveTowards(FollowLocation, DeltaTime);
        }

        this->UnitSetTriggerPulled(Distance < 600.0f);
    }
}
