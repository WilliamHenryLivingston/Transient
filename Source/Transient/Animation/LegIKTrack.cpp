// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKTrack.h"

#define DEBUG_DRAWS true

#ifdef DEBUG_DRAWS
#include "DrawDebugHelpers.h"
#endif

FLegIKTrackTickResult FLegIKTrack::LegIKTrackTick(float DeltaTime, USceneComponent* Parent, bool MayStep) {
    FVector ComponentWorldLocation = Parent->GetComponentLocation();
    FRotator ComponentRotation = Parent->GetComponentRotation();

    FLegIKTrackTickResult Result;
    Result.DidStep = false;

    float ReachComponentDistance = this->Config.StepDistance * this->Config.RestReturnReachCoef;

    FVector ContactGroundWorldLocation = this->IKGroundHit(this->CurrentWorldLocation, Parent);
    FVector TickTargetWorldLocation = this->CurrentWorldLocation;
    TickTargetWorldLocation.Z = ContactGroundWorldLocation.Z + this->Config.GroundVerticalOffset;

    if (this->StepPhase == EIKStepPhase::None && MayStep) {
        FVector RestWorldLocation = this->RestComponentLocation + ComponentWorldLocation;

        float CurrentStepDistance = (RestWorldLocation - this->CurrentWorldLocation).Size2D();
        if (CurrentStepDistance > this->Config.StepDistance || (this->ReturnToRest && CurrentStepDistance > ReachComponentDistance)) {
            this->TargetStepWorldLocation = RestWorldLocation;
            this->TargetStepWorldLocation.Z = this->IKGroundHit(this->TargetStepWorldLocation, Parent).Z;
            this->StepPhase = EIKStepPhase::Lift;
        }
    }
    else if (this->StepPhase == EIKStepPhase::Lift) {
        float LiftedZ = TickTargetWorldLocation.Z + this->Config.StepSwingVerticalOffset;
        if (FMath::Abs(this->CurrentWorldLocation.Z - LiftedZ) < 5.0f) {
            this->StepPhase = EIKStepPhase::Swing;
        }
        else {
            TickTargetWorldLocation.Z += this->Config.StepSwingVerticalOffset;
        }
    }
    else if (this->StepPhase == EIKStepPhase::Swing) {
        FVector PlaneDistance = this->CurrentWorldLocation - this->TargetStepWorldLocation;
        PlaneDistance.Z = 0.0f;
        if (PlaneDistance.Size() < ReachComponentDistance) {
            this->StepPhase = EIKStepPhase::Place;
        }
        else {
            TickTargetWorldLocation.X = this->TargetStepWorldLocation.X;
            TickTargetWorldLocation.Y = this->TargetStepWorldLocation.Y;
            TickTargetWorldLocation.Z += this->Config.StepSwingVerticalOffset;
        }
    }
    else if (this->StepPhase == EIKStepPhase::Place && FMath::Abs(this->CurrentWorldLocation.Z - TickTargetWorldLocation.Z) < 20.0f) { // Place.
        this->StepPhase = EIKStepPhase::None;
        Result.DidStep = true;
    }

    this->CurrentWorldLocation = FMath::VInterpTo(
        this->CurrentWorldLocation,
        TickTargetWorldLocation,
        DeltaTime, this->Config.LerpRate
    );
    Result.NewTarget = this->CurrentComponentLocation = FMath::VInterpTo(
        this->CurrentComponentLocation,
        ComponentRotation.UnrotateVector(this->CurrentWorldLocation - ComponentWorldLocation),
        DeltaTime, this->Config.LerpRate
    );

    #ifdef DEBUG_DRAWS
    DrawDebugSphere(
        Parent->GetWorld(),
        this->TargetStepWorldLocation,
        5.0f, 5,
        this->StepPhase != EIKStepPhase::None ? FColor::Yellow : FColor::Black,
        false, 0.2f, 100
    );
    DrawDebugSphere(
        Parent->GetWorld(),
        this->RestComponentLocation + ComponentWorldLocation,
        3.0f, 5, FColor::Orange, false, 0.2f, 100
    );
    #endif

    return Result;
}

FVector FLegIKTrack::IKGroundHit(FVector Below, USceneComponent* Parent) { // World space.
    FVector RayEnd = Below - FVector(0.0f, 0.0f, this->Config.GroundCastDistance);

    FHitResult HitResult;
    Parent->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Below, RayEnd,
        ECollisionChannel::ECC_Visibility,
        FCollisionQueryParams()
    );

    return HitResult.Location;
}
