// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKTrack.h"

//#define DEBUG_DRAWS true

#ifdef DEBUG_DRAWS
#include "DrawDebugHelpers.h"
#endif

FLegIKTrack::FLegIKTrack() { }

FLegIKTrack::FLegIKTrack(FVector InitialWorldLocation, FLegIKProfile InitProfile) {
    this->CurrentWorldLocation = InitialWorldLocation;
    this->Profile = InitProfile;
}

float FLegIKTrack::LegIKTrackStepProgress() {
    return FMath::Min(1.0f, this->StepTime / this->StepTimeBudget);
}

void FLegIKTrack::LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent, float TimeBudget) {
    this->StepStartWorldLocation = this->CurrentWorldLocation;
    this->StepWorldLocation = WorldOffset;
    this->StepWorldLocation.Z = this->GroundHit(this->StepWorldLocation, Parent).Z;
    this->StepTimeBudget = TimeBudget;
    this->StepTime = 0.0f;
}

FVector FLegIKTrack::LegIKTrackTick(float DeltaTime, USceneComponent* Parent, FLegIKDynamics Dynamics) {
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
        this->CurrentWorldLocation.Z = GroundHitZ + (sin(Alpha * 3.14159f) * this->Profile.StepVerticalOffset); 
    }

    #ifdef DEBUG_DRAWS
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

FVector FLegIKTrack::GroundHit(FVector Below, USceneComponent* Parent) {
    FVector RayEnd = Below - FVector(0.0f, 0.0f, this->Profile.GroundCastDistance);

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
