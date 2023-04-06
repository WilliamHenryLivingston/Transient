// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKTrack.h"

#define DEBUG_DRAWS true

#ifdef DEBUG_DRAWS
#include "DrawDebugHelpers.h"
#endif

FLegIKTrack::FLegIKTrack() { }

FLegIKTrack::FLegIKTrack(FVector InitialWorldLocation, FLegIKTrackConfig Config) {
    this->CurrentWorldLocation = InitialWorldLocation;
    this->Config = Config;   
}

FVector FLegIKTrack::LegIKTrackWorldLocation() {
    return this->CurrentWorldLocation;
}

bool FLegIKTrack::LegIKTrackIsStepping() {
    return this->StepPhase != ELegIKStepPhase::None && this->StepPhase != ELegIKStepPhase::Place;
}

void FLegIKTrack::LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent) {
    this->StepWorldLocation = WorldOffset;
    this->StepWorldLocation.Z = this->GroundHit(this->StepWorldLocation, Parent).Z;
    this->StepPhase = ELegIKStepPhase::Lift;
}

FVector FLegIKTrack::LegIKTrackTick(float DeltaTime, USceneComponent* Parent) {
    FVector TickTargetWorldLocation = this->CurrentWorldLocation;
    TickTargetWorldLocation.Z = (
        this->GroundHit(this->CurrentWorldLocation, Parent).Z +
        this->Config.GroundVerticalOffset
    );

    if (this->StepPhase == ELegIKStepPhase::Lift) {
        float LiftedZ = TickTargetWorldLocation.Z + this->Config.StepSwingVerticalOffset;
        if (FMath::Abs(this->CurrentWorldLocation.Z - LiftedZ) < 2.0f) {
            TickTargetWorldLocation.Z += this->Config.StepSwingVerticalOffset;
            this->StepPhase = ELegIKStepPhase::Swing;
        }
        else {
            FVector PartialStepWorldDelta = (
                this->StepWorldLocation - this->CurrentWorldLocation
            ) * 0.25f;
            TickTargetWorldLocation.Z += this->Config.StepSwingVerticalOffset;
            TickTargetWorldLocation.X += PartialStepWorldDelta.X;
            TickTargetWorldLocation.Y += PartialStepWorldDelta.Y;
        }
    }
    else if (this->StepPhase == ELegIKStepPhase::Swing) {
        FVector PlaneWorldDistance = this->CurrentWorldLocation - this->StepWorldLocation;
        PlaneWorldDistance.Z = 0.0f;
        if (PlaneWorldDistance.Size() < this->Config.StepReachWorldRadius) {
            this->StepPhase = ELegIKStepPhase::Place;
        }
        else {
            TickTargetWorldLocation.X = this->StepWorldLocation.X;
            TickTargetWorldLocation.Y = this->StepWorldLocation.Y;
            TickTargetWorldLocation.Z += this->Config.StepSwingVerticalOffset;
        }
    }
    else if (this->StepPhase == ELegIKStepPhase::Place) { // Place.
        float WorldDistance = FMath::Abs(this->CurrentWorldLocation.Z - TickTargetWorldLocation.Z);

        if (WorldDistance < this->Config.StepReachWorldRadius) {
            this->StepPhase = ELegIKStepPhase::None;
        }
    }

    this->CurrentWorldLocation = FMath::VInterpTo(
        this->CurrentWorldLocation,
        TickTargetWorldLocation,
        DeltaTime,
        // Not sure why this is needed, but behavior is wrong otherwise when global time != 1.
        this->Config.LerpRate / DeltaTime
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
    FVector RayEnd = Below - FVector(0.0f, 0.0f, this->Config.GroundCastDistance);

    FHitResult HitResult;
    Parent->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Below, RayEnd,
        ECollisionChannel::ECC_GameTraceChannel3,
        FCollisionQueryParams()
    );

    return HitResult.Location;
}
