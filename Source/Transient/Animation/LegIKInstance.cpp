// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

void ULegIKInstance::LegIKInstanceInit(USceneComponent* Parent, FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig) {
    this->Config = InitConfig;

    for (int i = 0; i < this->Config.LegCount; i++) {
        FLegIKTrack Track;
        Track.Config = TracksConfig;
        Track.StepPhase = EIKStepPhase::None;
        this->Tracks.Push(Track);
    }

    if (this->Config.PlacementStrategy == ELegIKPlacementStrategy::Circle) {
        float DegreeOffset = 360.0f / this->Config.LegCount;

        FRotator Rotator;
        FVector OffsetVector(0.0f, this->Config.LegRestComponentLocationOffset, 0.0f);

        Rotator.Yaw += DegreeOffset / 2.0f;

        for (int i = 0; i < this->Config.LegCount; i++) {
            FVector RestComponentLocation = Rotator.RotateVector(OffsetVector);
            this->RestComponentLocations.Push(this->Config.CenterOffset + RestComponentLocation);
            Rotator.Yaw += DegreeOffset;
        }
    }
    else if (this->Config.PlacementStrategy == ELegIKPlacementStrategy::Biped) {
        this->RestComponentLocations.Push(this->Config.CenterOffset + FVector(
            0.0f,
            this->Config.LegRestComponentLocationOffset,
            0.0f
        ));
        this->RestComponentLocations.Push(this->Config.CenterOffset + FVector(
            0.0f,
            -this->Config.LegRestComponentLocationOffset,
            0.0f
        ));
    }

    for (int i = 0; i < this->Tracks.Num(); i++) {
        this->Tracks[i].CurrentWorldLocation = Parent->GetComponentLocation() + this->RestComponentLocations[i];
    }
 }

void ULegIKInstance::LegIKInstanceTick(float DeltaTime, USceneComponent* Parent) {
    FVector CurrentWorldLocation = Parent->GetComponentLocation();
    FRotator CurrentRotation = Parent->GetComponentRotation();
    FVector TickMoveDelta = CurrentWorldLocation - this->LastWorldLocation;
    this->LastWorldLocation = CurrentWorldLocation;

    int LocationSamples = 2;
    if (this->LastMoveDeltas.Num() < LocationSamples) {
        this->LastMoveDeltas.Push(CurrentWorldLocation);
        this->LastMoveDeltaIndex = this->LastMoveDeltas.Num() - 1;
    }
    else {
        this->LastMoveDeltaIndex = (this->LastMoveDeltaIndex + 1) % LocationSamples;
        this->LastMoveDeltas[this->LastMoveDeltaIndex] = TickMoveDelta;
    }

    FVector MoveWorldDelta;
    for (int i = 0; i < this->LastMoveDeltas.Num(); i++) {
        MoveWorldDelta += this->LastMoveDeltas[i] / LocationSamples;
    }
    MoveWorldDelta.Z = 0;

    for (int i = 0; i < this->Config.LegCount; i++) {
        this->Tracks[i].ReturnToRest = false;
        this->Tracks[i].RestComponentLocation = CurrentRotation.RotateVector(this->RestComponentLocations[i]);
    }

    // Find nearest rest position to direction of travel. That group becomes the
    // leading group and it's rest positions are adjusted accordingly.
    bool Moving = !MoveWorldDelta.IsZero();
    if (Moving) {
        FVector MoveNormalizedDelta = MoveWorldDelta.GetSafeNormal();
        FVector RestRelativeMoveComponentDelta = MoveNormalizedDelta * this->Config.LegRestComponentLocationOffset;

        int ClosestIndex = -1;
        float ClosestDistance = 0.0f;
        for (int i = 0; i < this->Config.LegCount; i++) {
            float CheckDistance = (this->RestComponentLocations[i] - RestRelativeMoveComponentDelta).Size();
            if (ClosestIndex < 0 || CheckDistance < ClosestDistance) {
                ClosestIndex = i;
                ClosestDistance = CheckDistance;
            }
        }

        bool GroupAClosest = this->Config.LegGroupA.Contains(ClosestIndex);

        for (int i = 0; i < this->Config.LegCount; i++) {
            this->Tracks[i].RestComponentLocation += (MoveWorldDelta * this->Config.TravelDirectionComponentOffset);
        }
    }

    // TODO: Improve this.
    bool CanGroupAStep = true;
    bool CanGroupBStep = true;
    for (int i = 0; i < this->Config.LegCount; i++) {
        if (this->Tracks[i].StepPhase != EIKStepPhase::None) {
            if (this->Config.LegGroupA.Contains(i)) CanGroupAStep = false;
            else CanGroupBStep = false;
        }
    }

    if (!Moving && CanGroupAStep && CanGroupBStep) {
        for (int i = 0; i < this->Config.LegCount; i++) {
            this->Tracks[i].ReturnToRest = true;
        }
    }

    // Tick tracks.
    TArray<FVector> NewIKTargets;
    bool DidAnyStep = false;
    for (int i = 0; i < this->Config.LegCount; i++) {
        bool MayStep = (
            this->Config.LegGroupA.Contains(i) ?
                CanGroupAStep && this->GroupAStepNext
                :
                CanGroupBStep && !this->GroupAStepNext
        );

        FLegIKTrackTickResult Result = this->Tracks[i].LegIKTrackTick(DeltaTime, Parent, MayStep);
        NewIKTargets.Push(Result.NewTarget);

        if (Result.DidStep) DidAnyStep = true;
    }
    if (DidAnyStep) this->GroupAStepNext = !this->GroupAStepNext;

    // Ensure torso Z offset constraints.
    // Careful! We're dealing with a negative offset here so signs are inverted.
    float ZComponentMax = NewIKTargets[0].Z;
    float ZComponentMin = NewIKTargets[0].Z;
    for (int i = 1; i < this->Config.LegCount; i++) {
        float CheckZ = NewIKTargets[i].Z;
        if (CheckZ > ZComponentMax) ZComponentMax = CheckZ;
        if (CheckZ < ZComponentMin) ZComponentMin = CheckZ;
    }

    FVector ActorWorldLocation = Parent->GetOwner()->GetActorLocation();

    float TorsoDelta = this->Config.TorsoLerpSpeed * DeltaTime;
    float RestOffsetPadding = 6.0f; // TODO.

    if (ZComponentMax > -this->Config.TorsoMinOffset) {
        ActorWorldLocation.Z += TorsoDelta;
    }
    else if (ZComponentMin < -this->Config.TorsoMaxOffset) {
        ActorWorldLocation.Z -= TorsoDelta;
    }
    else if (FMath::Abs(FMath::Abs(ZComponentMax) - FMath::Abs(ZComponentMin)) < RestOffsetPadding) {
        if (ZComponentMax > -(this->Config.TorsoRestOffset - RestOffsetPadding)) {
            ActorWorldLocation.Z += TorsoDelta;
        }
        else if (ZComponentMin < -(this->Config.TorsoRestOffset + RestOffsetPadding)) {
            ActorWorldLocation.Z -= TorsoDelta;
        }
    }

    this->Script_IKTargets = NewIKTargets;
    Parent->GetOwner()->SetActorLocation(ActorWorldLocation);
}
