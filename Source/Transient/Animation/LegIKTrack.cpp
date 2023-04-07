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

FVector FLegIKTrack::LegIKTrackWorldLocation() { return this->CurrentWorldLocation; }
ELegIKStepPhase FLegIKTrack::LegIKTrackGetStepPhase() { return this->StepPhase; }

void FLegIKTrack::LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent) {
    this->StepWorldLocation = WorldOffset;
    this->StepWorldLocation.Z = this->GroundHit(this->StepWorldLocation, Parent).Z;
    this->StepPhase = ELegIKStepPhase::Lift;
}

FVector FLegIKTrack::LegIKTrackTick(
    float DeltaTime, USceneComponent* Parent, FLegIKDynamics Dynamics
) {
    FVector TickTargetWorldLocation = this->CurrentWorldLocation;
    TickTargetWorldLocation.Z = this->GroundHit(this->CurrentWorldLocation, Parent).Z;

    float StepVerticalOffset = this->Profile.StepVerticalOffset * Dynamics.StepVerticalCoef;

    if (this->StepPhase == ELegIKStepPhase::Lift) {
        float LiftedZ = TickTargetWorldLocation.Z + StepVerticalOffset;

        if (FMath::Abs(this->CurrentWorldLocation.Z - LiftedZ) < 5.0f) {
            TickTargetWorldLocation.Z += StepVerticalOffset;
            this->StepPhase = ELegIKStepPhase::Swing;
        }
        else {
            FVector PartialStepWorldDelta = (
                (this->StepWorldLocation - this->CurrentWorldLocation) * 0.25f
            );
            TickTargetWorldLocation.Z += StepVerticalOffset;
            TickTargetWorldLocation.X += PartialStepWorldDelta.X;
            TickTargetWorldLocation.Y += PartialStepWorldDelta.Y;
        }
    }
    else if (this->StepPhase == ELegIKStepPhase::Swing) {
        FVector PlaneWorldDistance = this->CurrentWorldLocation - this->StepWorldLocation;
        PlaneWorldDistance.Z = 0.0f;

        if (PlaneWorldDistance.Size() < this->Profile.StepReachWorldRadius) {
            this->StepPhase = ELegIKStepPhase::Place;
        }
        else {
            TickTargetWorldLocation.X = this->StepWorldLocation.X;
            TickTargetWorldLocation.Y = this->StepWorldLocation.Y;
            TickTargetWorldLocation.Z += StepVerticalOffset;
        }
    }
    else if (this->StepPhase == ELegIKStepPhase::Place) {
        float WorldDistance = FMath::Abs(this->CurrentWorldLocation.Z - TickTargetWorldLocation.Z);

        if (WorldDistance < this->Profile.StepReachWorldRadius) {
            this->StepPhase = ELegIKStepPhase::None;
        }
    }

    this->CurrentWorldLocation = FMath::VInterpTo(
        this->CurrentWorldLocation,
        TickTargetWorldLocation,
        DeltaTime,
        this->Profile.LerpRate * Dynamics.LerpRateCoef
    );

    #ifdef DEBUG_DRAWS
    DrawDebugSphere(
        Parent->GetWorld(),
        this->StepWorldLocation,
        5.0f, 5,
        this->StepPhase != ELegIKStepPhase::None ? FColor::Yellow : FColor::Black,
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
