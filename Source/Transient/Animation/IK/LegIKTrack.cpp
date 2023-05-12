// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKTrack.h"

FLegIKTrack::FLegIKTrack(FVector InitBaseLocation, UUnitAgent* InitParent) {
    this->BaseLocation = InitBaseLocation;
    this->ParentUnit = InitParent;

    this->CurrentWorldLocation = (
        this->ParentUnit->GetActorLocation() +
        this->ParentUnit->GetActorRotation().RotateVector(this->BaseLocation)
    );
}

FVector FLegIKTrack::TrackCurrentLocation() { return this->CurrentWorldLocation; }

float FLegIKTrack::TrackStepErrorDistance(FVector GivenVelocity) {
    if (this->StepTime >= this->StepTimeBudget) return 0.0f;

    FVector RecheckedEndLocation = (
        this->ParentUnit->GetActorLocation() +
        (GivenVelocity * this->StepTimeBudget * (1.0f - TrackProgress)) +
        this->ParentUnit->GetActorRotation().RotateVector(this->BaseLocation)
    );

    return (RecheckedEndLocation - this->StepWorldLocation).Size2D();
}

void FLegIKTrack::TrackStepTo(FVector WorldOffset, USceneComponent* Parent, float TimeBudget) {
    this->StepStartWorldLocation = this->CurrentWorldLocation;
    this->StepWorldLocation = WorldOffset;
    this->StepWorldLocation.Z = this->GroundHit(this->StepWorldLocation, Parent).Z;
    this->StepTimeBudget = TimeBudget;
    this->StepTime = 0.0f;
}

FVector FLegIKTrack::TrackTick(float DeltaTime, USceneComponent* Parent) {
    float GroundHitZ = this->GroundHit(this->CurrentWorldLocation, Parent).Z;

    if (this->StepTime >= this->StepTimeBudget) {
        this->CurrentWorldLocation = this->StepWorldLocation;
        this->CurrentWorldLocation.Z = GroundHitZ;
    }
    else {
        this->StepTime += DeltaTime;
        float Alpha = this->StepTime / this->StepTimeBudget;

        this->CurrentWorldLocation = FMath::Lerp(
            this->StepStartWorldLocation,
            this->StepWorldLocation,
            Alpha
        );
        this->CurrentWorldLocation.Z = GroundHitZ + (sin(Alpha * 3.14159f) * this->ParentUnit.LegStepVerticalOffset); 
    }

    #if DEBUG_IK
    DrawDebugSphere(
        Parent->GetWorld(),
        this->StepWorldLocation,
        5.0f, 5,
        this->StepTime >= this->StepTimeBudget ? FColor::Yellow : FColor::Black,
        false, 0.2f, 100
    );
    DrawDebugSphere(
        Parent->GetWorld(),
        this->CurrentWorldLocation,
        3.0f, 5, FColor::Orange, false, 0.2f, 100
    );
    #endif

    return this->CurrentWorldLocation;
}

FVector FLegIKTrack::TrackGroundHit(FVector Below, USceneComponent* Parent) {
    FVector RayEnd = Below - FVector(0.0f, 0.0f, this->ParentUnit.LegGroundCastDistance);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Parent->GetOwner());

    FHitResult HitResult;
    Parent->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Below, RayEnd,
        ECollisionChannel::ECC_GameTraceChannel4,
        Params
    );

    return HitResult.Location;
}
