// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

void ULegIKInstance::LegIKInstanceInit(USceneComponent* Parent, FLegIKProfile InitProfile) {
    this->Profile = InitProfile;
    this->Dynamics = FLegIKDynamics();

    TArray<FLegIKTrackGroupProfile> GroupProfiles = this->Profile.TrackGroups;
    this->TrackGroups = TArray<FLegIKTrackGroup>();

    FVector ParentLocation = Parent->GetComponentLocation();
    FRotator BodyRotation = Parent->GetComponentRotation();
    for (int i = 0; i < GroupProfiles.Num(); i++) {
        FLegIKTrackGroupProfile* GroupProfile = &GroupProfiles[i];
        FLegIKTrackGroup Group;

        for (int j = 0; j < GroupProfile->BaseComponentLocations.Num(); j++) {
            FVector BaseComponentLocation = GroupProfile->BaseComponentLocations[j];

            Group.Tracks.Push(FLegIKTrack(
                ParentLocation + BodyRotation.RotateVector(BaseComponentLocation),
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

// TODO: Optimize and refactor.
void ULegIKInstance::LegIKInstanceTick(float RawDeltaTime, USceneComponent* Parent) {
    FVector BodyLocation = Parent->GetComponentLocation();
    FRotator BodyRotation = Parent->GetComponentRotation();
    FVector BodyVelocity = this->Dynamics.BodyVelocity;

    float DeltaTime = RawDeltaTime * Dynamics.DeltaTimeCoef;
    float CurrentStepDistance = this->Profile.StepBaseDistance * Dynamics.StepDistanceCoef;
    float CurrentOffsetTolerance = this->Profile.RestingOffsetTolerance;
    bool BodyMoving = BodyVelocity.Size2D() > 10.0f;
    if (BodyMoving) {
        CurrentOffsetTolerance = this->Profile.MovingOffsetTolerance;
    }

    bool ConsiderStep = true;
    TArray<FLegIKTrackGroup*> ActiveStepGroups;
    TArray<bool> GroupsStillPlacing;
    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        bool StillPlacing = false;
        for (int j = 0; j < Group->Tracks.Num(); j++) {
            float CheckProgress = Group->Tracks[j].LegIKTrackStepProgress();
            if (CheckProgress >= 1.0f) continue;
            
            if (!ActiveStepGroups.Contains(Group)) ActiveStepGroups.Push(Group);

            if (CheckProgress >= 0.8f) StillPlacing = true;
            else ConsiderStep = false;
        }
        GroupsStillPlacing.Push(StillPlacing);
    }

    // Validate existing steps, scheduling any that are invalid for reset.
    // TODO: Simplify as: did currentworldposition change?
    TArray<FLegIKTrackGroup*> TriggerStepGroups;
    for (int i = 0; i < ActiveStepGroups.Num(); i++) {
        FLegIKTrackGroup* CheckGroup = ActiveStepGroups[i];

        for (int j = 0; j < CheckGroup->Tracks.Num(); j++) {
            FLegIKTrack* CheckTrack = &CheckGroup->Tracks[j];
            float TrackProgress = CheckTrack->LegIKTrackStepProgress();
            if (TrackProgress >= 1.0f) continue;

            FVector ExpectedEndLocation = (
                BodyLocation +
                (BodyVelocity * CheckTrack->StepTimeBudget * (1.0f - TrackProgress)) +
                BodyRotation.RotateVector(CheckGroup->BaseComponentLocations[j])
            );

            float DistanceToOriginalTarget = (
                (ExpectedEndLocation - CheckTrack->StepWorldLocation).Size2D()
            );
            bool Invalidated = DistanceToOriginalTarget > (CurrentOffsetTolerance * 2.0f);

            #if DEBUG_IK
            DrawDebugSphere(
                Parent->GetWorld(),
                ExpectedEndLocation,
                5.0f, 5,
                Invalidated ? FColor::Red : FColor::Purple,
                false, 0.2f, 100
            );
            #endif

            if (Invalidated) {
                TriggerStepGroups.Push(CheckGroup);
                break;
            }
        }
    }
    
    // Update current world location tracking.
    FVector StepTargetDelta;
    if (BodyMoving) {
        StepTargetDelta += BodyVelocity.GetSafeNormal() * 1.25f * CurrentStepDistance;
    }

    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        Group->CurrentWorldLocations = TArray<FVector>();
        for (int j = 0; j < Group->Tracks.Num(); j++) { 
            FVector NewWorldLocation = (
                BodyLocation +
                BodyRotation.RotateVector(Group->BaseComponentLocations[j]) +
                StepTargetDelta
            );

            #if DEBUG_IK
            DrawDebugSphere(
                Parent->GetWorld(),
                NewWorldLocation,
                5.0f, 5, FColor::Green, false, 0.2f, 100
            );
            #endif

            Group->CurrentWorldLocations.Push(NewWorldLocation);
        }
    }

    if (ConsiderStep) {
        FLegIKTrackGroup* MaxGroup = nullptr;
        float MaxOffset = CurrentOffsetTolerance;
        for (int i = 0; i < this->TrackGroups.Num(); i++) {
            if (GroupsStillPlacing[i]) continue;

            FLegIKTrackGroup* CheckGroup = &this->TrackGroups[i];

            float CheckOffset = FMath::Abs((
                CheckGroup->Tracks[0].CurrentWorldLocation -
                CheckGroup->CurrentWorldLocations[0]
            ).Size2D());

            if (CheckOffset > MaxOffset) {
                MaxOffset = CheckOffset;
                MaxGroup = CheckGroup;
            }
        }
        if (MaxGroup != nullptr) TriggerStepGroups.Push(MaxGroup);
    }

    // Trigger steps.
    float TimeBudget = 0.2f;
    if (BodyMoving) TimeBudget = CurrentStepDistance / BodyVelocity.Size();
    // TODO: Conf.
    if (TimeBudget > 0.8f) TimeBudget = 0.8f;

    for (int i = 0; i < TriggerStepGroups.Num(); i++) {
        FLegIKTrackGroup* Group = TriggerStepGroups[i];

        for (int j = 0; j < Group->Tracks.Num(); j++) {
            Group->Tracks[j].LegIKTrackStepTo(
                Group->CurrentWorldLocations[j],
                Parent,
                TimeBudget
            );
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
                BodyRotation.UnrotateVector(TargetWorldLocation - BodyLocation)
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
        this->Profile.LerpRate * this->Dynamics.BodyLerpRateCoef
    );

    this->Script_IKTargets.Push(InterpedRootTarget);
}
