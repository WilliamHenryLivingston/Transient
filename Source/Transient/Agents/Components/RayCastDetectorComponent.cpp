// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "RayCastDetectorComponent.h"

#include "Transient/Common.h"

TArray<AAgentActor*> URayCastDetectorComponent::DetectorTick(float DeltaTime) {
    UWorld World = this->GetWorld();

    FVector DetectorLocation = this->GetComponentLocation();
    FVector LocalRay = FVector(this->DetectionDistance, 0.0f, 0.0f);
    FRotator DetectorRotation = this->GetComponentRotation();

    int HalfRayCount = this->RayCount / 2;

    TArray<AAgentActor*> Hits;
    for (int i = 0; i < this->RayCount; i++) {
        FRotator RayRotation = DetectorRotation + (this->RayOffset * (i - HalfRayCount));
        FVector DetectorRayEnd = DetectorLocation + RayRotation.RotateVector(LocalRay);

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this->GetOwner());

        FHitResult DetectionHit;
        World->LineTraceSingleByChannel(
            DetectionHit,
            DetectorLocation, DetectorRayEnd,
            ECollisionChannel::ECC_Visibility,
            Params
        );

        AAgentActor* HitAgent = Cast<AAgentActor>(DetectionHit.GetActor());

        #if DEBUG_DETECTION
        DrawDebugLine(
            World,
            DetectorLocation, DetectionHit.ImpactPoint,
            HitAgent == nullptr ? FColor::Blue : FColor::Red,
            false, 1.0f, 0, 0.1f
        );
        #endif

        if (HitAgent != nullptr) Hits.Push(HitAgent);
    }

    return Hits;
}
