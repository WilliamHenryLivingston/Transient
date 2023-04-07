// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

//#define DEBUG_DRAWS true

// fixed step lengths rather than delta time based.

void ULegIKInstance::LegIKInstanceInit(
    USceneComponent* Parent,
    FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig
) {
    this->LastWorldLocation = Parent->GetComponentLocation();
    this->Config = InitConfig;

    this->DynamicMoveTargetingCoef = 1.0f;
    this->DynamicBodyBaseOffsetCoef = 1.0f;

    // Compute base placements.
    TArray<FVector> BaseComponentLocations;
    if (this->Config.LegPlacementStrategy == ELegIKPlacementStrategy::Circle) {
        float DegreeOffset = 360.0f / this->Config.LegCount;

        FRotator Rotator;
        FVector OffsetVector = FVector(0.0f, this->Config.LegBaseComponentOffset, 0.0f);

        Rotator.Yaw += DegreeOffset / 2.0f;

        for (int i = 0; i < this->Config.LegCount; i++) {
            BaseComponentLocations.Push(Rotator.RotateVector(OffsetVector));
            Rotator.Yaw += DegreeOffset;
        }
    }
    else if (this->Config.LegPlacementStrategy == ELegIKPlacementStrategy::Biped) {
        FVector SideOffset = FVector(0.0f, this->Config.LegBaseComponentOffset, 0.0f);

        BaseComponentLocations.Push(SideOffset);
        BaseComponentLocations.Push(SideOffset * -1.0f);
    }

    // Create tracks.
    TArray<FLegIKTrack> Tracks;
    FVector ParentWorldLocation = Parent->GetComponentLocation();
    for (int i = 0; i < this->Config.LegCount; i++) {
        Tracks.Push(FLegIKTrack(
            ParentWorldLocation + BaseComponentLocations[i],
            TracksConfig
        ));
    }

    // Create track groups.
    for (int i = 0; i < this->Config.LegGroups.Num(); i++) {
        FLegIKTrackGroupConfig GroupConfig = this->Config.LegGroups[i];
        FLegIKTrackGroup Group;

        for (int j = 0; j < GroupConfig.Members.Num(); j++) {
            int LegIndex = GroupConfig.Members[j];

            Group.Tracks.Push(Tracks[LegIndex]);
            Group.BaseComponentLocations.Push(BaseComponentLocations[LegIndex]);
        }

        this->TrackGroups.Push(Group);
    }
}

void ULegIKInstance::LegIKInstanceSetDynamics(
    float NewLerpRate, float NewMoveTargetingCoef,
    float NewDynamicBodyBaseOffsetCoef,
    float NewDynamicStepVerticalCoef
) {
    this->DynamicMoveTargetingCoef = NewMoveTargetingCoef;
    this->DynamicBodyBaseOffsetCoef = NewDynamicBodyBaseOffsetCoef;

    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        for (int j = 0; j < Group->Tracks.Num(); j++) {
            Group->Tracks[j].DynamicLerpRateCoef = NewLerpRate;
            Group->Tracks[j].DynamicStepVerticalCoef = NewDynamicStepVerticalCoef;
        }
    }
}

void ULegIKInstance::LegIKInstanceTick(float DeltaTime, USceneComponent* Parent) {
    FVector ParentWorldLocation = Parent->GetComponentLocation();
    FRotator ParentRotation = Parent->GetComponentRotation();

    FVector MoveWorldDelta = ParentWorldLocation - this->LastWorldLocation;
    this->LastWorldLocation = ParentWorldLocation;

    float AccelerationSize = (this->LastMoveDelta - MoveWorldDelta).Size2D();
    bool CriticalDeltaChange = AccelerationSize > this->Config.DeltaChangeTolerance;
    this->LastMoveDelta = MoveWorldDelta;
    
    #ifdef DEBUG_DRAWS
    if (CriticalDeltaChange) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, MoveWorldDelta.ToString());
    #endif

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
        float CurrentOffsetTolerance = this->Config.RestingOffsetTolerance;
        FVector MoveRetargetDelta;
        if (!MoveWorldDelta.IsZero()) {
            CurrentOffsetTolerance = this->Config.MovingOffsetTolerance;
            MoveRetargetDelta += (
                MoveWorldDelta *
                this->DynamicMoveTargetingCoef *
                this->Config.MoveTargetingCoef
            );
            MoveRetargetDelta /= FMath::Sqrt(MoveRetargetDelta.Size());
        }

        for (int i = 0; i < this->TrackGroups.Num(); i++) {
            FLegIKTrackGroup* Group = &this->TrackGroups[i];

            Group->CurrentWorldLocations = TArray<FVector>();
            for (int j = 0; j < Group->Tracks.Num(); j++) { 
                FVector NewWorldLocation = (
                    ParentWorldLocation +
                    ParentRotation.RotateVector(Group->BaseComponentLocations[j]) +
                    MoveRetargetDelta +
                    FVector(0.0f, 0.0f, 20.0f)
                );

                // TODO: Not here...
                bool BipedCrouch = (
                    this->DynamicBodyBaseOffsetCoef < 1.0f &&
                    this->Config.LegPlacementStrategy == ELegIKPlacementStrategy::Biped
                );
                if (BipedCrouch) {
                    FVector OffsetVector = FVector(this->Config.LegBaseComponentOffset, 0.0f, 0.0f);
            
                    if (i == 0) NewWorldLocation += OffsetVector;
                    else NewWorldLocation -= OffsetVector;
                }
 
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
                ).Size());

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
            FVector TargetWorldLocation = Group->Tracks[j].LegIKTrackTick(DeltaTime, Parent);
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
    this->Script_IKTargets = NewIKTargets;

    // Body manipulation; compute root bone target (last target element).
    // TODO: Finish (adjust for foot Z placements).
    float ZComponentMax = NewIKTargets[0].Z;
    float ZComponentMin = NewIKTargets[0].Z;
    for (int i = 1; i < this->Config.LegCount; i++) {
        float CheckZ = NewIKTargets[i].Z;
        if (CheckZ > ZComponentMax) ZComponentMax = CheckZ;
        if (CheckZ < ZComponentMin) ZComponentMin = CheckZ;
    }

    FVector TickRootTarget = FVector(
        0.0f, 0.0f,
        (this->Config.BodyRestOffset - ZComponentMin) * this->DynamicBodyBaseOffsetCoef
    );

    FVector InterpedRootTarget = FMath::VInterpTo(
        LastRootTarget,
        TickRootTarget,
        DeltaTime,
        // Not sure why this is needed, but behavior is wrong otherwise when global time != 1.
        this->Config.BodyLerpRate / DeltaTime
    );

    this->Script_IKTargets.Push(InterpedRootTarget);
}
