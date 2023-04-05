// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

void ULegIKInstance::LegIKInstanceInit(FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig) {
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
 }

void ULegIKInstance::LegIKInstanceTick(float DeltaTime, USceneComponent* Parent) {
    FVector CurrentWorldLocation = Parent->GetComponentLocation();
    FVector MoveWorldDelta = CurrentWorldLocation - this->LastWorldLocation;
    this->LastWorldLocation = CurrentWorldLocation;
    MoveWorldDelta.Z = 0;

    for (int i = 0; i < this->Config.LegCount; i++) {
        this->Tracks[i].RestComponentLocation = this->RestComponentLocations[i];
    }

    // Find nearest rest position to direction of travel. That group becomes the
    // leading group and it's rest positions are adjusted accordingly.
    if (!MoveWorldDelta.IsZero()) {
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

        // TODO: Better vals.
        FVector LeadGroupComponentOffset = FVector();//MoveNormalizedDelta * this->Tracks[0].Config.StepDistance;
        FVector FollowGroupsComponentOffset = FVector();

        for (int i = 0; i < this->Config.LegCount; i++) {
            bool IsLeading = GroupAClosest == this->Config.LegGroupA.Contains(i);
            this->Tracks[i].RestComponentLocation += (
                IsLeading ? LeadGroupComponentOffset : FollowGroupsComponentOffset
            );
        }
    }

    // TODO: Improve this.
    bool CanGroupAStep = this->GroupAStepNext;
    bool CanGroupBStep = !this->GroupAStepNext;
    for (int i = 0; i < this->Config.LegCount; i++) {
        if (this->Tracks[i].StepPhase != EIKStepPhase::None) {
            if (this->Config.LegGroupA.Contains(i)) CanGroupAStep = false;
            else CanGroupBStep = false;
        }
    }

    // Tick tracks.
    TArray<FVector> NewIKTargets;
    for (int i = 0; i < this->Config.LegCount; i++) {
        bool MayStep = this->Config.LegGroupA.Contains(i) ? CanGroupAStep : CanGroupBStep;

        FLegIKTrackTickResult Result = this->Tracks[i].LegIKTrackTick(DeltaTime, Parent, MayStep);
        NewIKTargets.Push(Result.NewTarget);

        if (Result.DidStep) this->GroupAStepNext = !this->GroupAStepNext;
    }

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
