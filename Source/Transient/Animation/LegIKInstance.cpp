// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

//#define DEBUG_DRAWS true

void ULegIKInstance::LegIKInstanceInit(USceneComponent* Parent, FLegIKProfile InitProfile) {
    this->Profile = InitProfile;
    this->Dynamics = FLegIKDynamics();
    this->LastWorldLocation = Parent->GetComponentLocation();

    TArray<FLegIKTrackGroupProfile> GroupProfiles = this->Profile.TrackGroups;
    this->TrackGroups = TArray<FLegIKTrackGroup>();

    FVector ParentLocation = Parent->GetComponentLocation();
    FRotator ParentRotation = Parent->GetComponentRotation();
    for (int i = 0; i < GroupProfiles.Num(); i++) {
        FLegIKTrackGroupProfile* GroupProfile = &GroupProfiles[i];
        FLegIKTrackGroup Group;

        for (int j = 0; j < GroupProfile->BaseComponentLocations.Num(); j++) {
            FVector BaseComponentLocation = GroupProfile->BaseComponentLocations[j];

            Group.Tracks.Push(FLegIKTrack(
                ParentLocation + ParentRotation.RotateVector(BaseComponentLocation),
                this->Profile
            ));
            Group.BaseComponentLocations.Push(BaseComponentLocation);
        }

        this->TrackGroups.Push(Group);
    }
}

void ULegIKInstance::LegIKInstanceSetDynamics(FLegIKDynamics NewDynamics) {
    this->Dynamics = NewDynamics;
}

void ULegIKInstance::LegIKInstanceTick(float DeltaTime, USceneComponent* Parent) {
    FVector ParentWorldLocation = Parent->GetComponentLocation();
    FRotator ParentRotation = Parent->GetComponentRotation();

    FVector TickMoveWorldDelta = ParentWorldLocation - this->LastWorldLocation;
    this->LastWorldLocation = ParentWorldLocation;
    TickMoveWorldDelta.Z = 0;

    if (this->LastMoveDeltas.Num() < 5) {
        this->LastMoveDeltas.Push(TickMoveWorldDelta);
        this->LastMoveDeltaIndex++;
    }
    else {
        this->LastMoveDeltaIndex = (this->LastMoveDeltaIndex + 1) % this->LastMoveDeltas.Num();
        this->LastMoveDeltas[this->LastMoveDeltaIndex] = TickMoveWorldDelta;
    }

    FVector MoveWorldDelta = TickMoveWorldDelta;
    if (Cast<UPrimitiveComponent>(Parent)->GetPhysicsAngularVelocityInDegrees().Size() > 10.0f) {
        for (int i = 0; i < this->LastMoveDeltas.Num(); i++) {
            MoveWorldDelta += this->LastMoveDeltas[i] / this->LastMoveDeltas.Num();
        }
    }

    #ifdef DEBUG_DRAWS
    DrawDebugLine(
        Parent->GetWorld(),
        ParentWorldLocation,
        ParentWorldLocation + (MoveWorldDelta * 1.0f),
        FColor::Blue,
        false, 1.0f, -1, 1.0f
    );
    #endif

    bool CriticalDeltaChange = (
        (TickMoveWorldDelta.IsZero() && !this->LastMoveDelta.IsZero()) ||
        acosf(FVector::DotProduct(TickMoveWorldDelta, this->LastMoveDelta)) > 2.0f
    );
    this->LastMoveDelta = TickMoveWorldDelta;

    FLegIKTrackGroup* CriticalImpactedGroup = nullptr;
    bool ConsiderStep = true;
    TArray<bool> GroupsStillPlacing;
    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        bool StillPlacing = false;
        for (int j = 0; j < Group->Tracks.Num(); j++) {
            ELegIKStepPhase CheckPhase = Group->Tracks[j].LegIKTrackGetStepPhase();
            if (CheckPhase == ELegIKStepPhase::None) continue;

            CriticalImpactedGroup = Group;
            if (CheckPhase == ELegIKStepPhase::Place) StillPlacing = true;
            else ConsiderStep = false;
        }
        GroupsStillPlacing.Push(StillPlacing);
    }

    if (ConsiderStep || CriticalDeltaChange) {
        // Compute current base positions and offset tolerance given current travel and
        // find group that needs to step most.
        float CurrentOffsetTolerance = this->Profile.RestingOffsetTolerance;
        FVector StepTargetDelta;
        if (MoveWorldDelta.Size2D() > 2.0f) {
            CurrentOffsetTolerance = this->Profile.MovingOffsetTolerance;
            StepTargetDelta += (
                MoveWorldDelta.GetSafeNormal() *
                this->Profile.StepBaseDistance *
                Dynamics.StepDistanceCoef *
                DeltaTime
            );
        }

        for (int i = 0; i < this->TrackGroups.Num(); i++) {
            FLegIKTrackGroup* Group = &this->TrackGroups[i];

            Group->CurrentWorldLocations = TArray<FVector>();
            for (int j = 0; j < Group->Tracks.Num(); j++) { 
                FVector NewWorldLocation = (
                    ParentWorldLocation +
                    ParentRotation.RotateVector(Group->BaseComponentLocations[j]) +
                    StepTargetDelta +
                    FVector(0.0f, 0.0f, 20.0f)
                );
 
                #ifdef DEBUG_DRAWS
                DrawDebugSphere(
                    Parent->GetWorld(),
                    NewWorldLocation,
                    5.0f, 5, FColor::Green, false, 0.2f, 100
                );
                #endif

                Group->CurrentWorldLocations.Push(NewWorldLocation);
            }
        }

        FLegIKTrackGroup* StepGroup = nullptr;
        if (CriticalDeltaChange) StepGroup = CriticalImpactedGroup;
        else {
            float MaxOffset = CurrentOffsetTolerance;
            for (int i = 0; i < this->TrackGroups.Num(); i++) {
                if (GroupsStillPlacing[i]) continue;

                FLegIKTrackGroup* CheckGroup = &this->TrackGroups[i];

                float CheckOffset = FMath::Abs((
                    CheckGroup->Tracks[0].LegIKTrackWorldLocation() -
                    CheckGroup->CurrentWorldLocations[0]
                ).Size2D());

                if (CheckOffset > MaxOffset) {
                    MaxOffset = CheckOffset;
                    StepGroup = CheckGroup;
                }
            }
        }

        // Perform step.
        if (StepGroup != nullptr) {
            for (int i = 0; i < StepGroup->Tracks.Num(); i++) {
                StepGroup->Tracks[i].LegIKTrackStepTo(
                    StepGroup->CurrentWorldLocations[i],
                    Parent
                );
            }
        }
    }

    // Tick tracks.
    TArray<FVector> NewIKTargets;
    bool DidAnyStep = false;
    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        for (int j = 0; j < Group->Tracks.Num(); j++) {
            FVector TargetWorldLocation = Group->Tracks[j].LegIKTrackTick(DeltaTime, Parent, this->Dynamics);
            FVector TargetComponentLocation = (
                ParentRotation.UnrotateVector(TargetWorldLocation - ParentWorldLocation)
            );

            NewIKTargets.Push(TargetComponentLocation);
        }
    }

    FVector LastRootTarget;
    if (this->Script_IKTargets.Num() > 0) {
        LastRootTarget = this->Script_IKTargets[this->Script_IKTargets.Num() - 1];
    }
    if (NewIKTargets.Num() == 0) return;
    this->Script_IKTargets = NewIKTargets;

    // Body manipulation; compute root bone target (last target element).
    // TODO: Finish (adjust for foot Z placements).
    float ZComponentMax = NewIKTargets[0].Z;
    float ZComponentMin = NewIKTargets[0].Z;
    for (int i = 1; i < NewIKTargets.Num(); i++) {
        float CheckZ = NewIKTargets[i].Z;
        if (CheckZ > ZComponentMax) ZComponentMax = CheckZ;
        if (CheckZ < ZComponentMin) ZComponentMin = CheckZ;
    }

    FVector TickRootTarget = FVector(
        0.0f, 0.0f,
        (this->Profile.BodyBaseOffset) * this->Dynamics.BodyBaseCoef
    );

    FVector InterpedRootTarget = FMath::VInterpTo(
        LastRootTarget, TickRootTarget,
        DeltaTime,
        this->Profile.LerpRate * this->Dynamics.LerpRateCoef
    );

    this->Script_IKTargets.Push(InterpedRootTarget);
}
