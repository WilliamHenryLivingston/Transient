// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKInstance.h"

void ULegIKInstance::LegIKInstanceInit(
    USceneComponent* Parent,
    FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig
) {
    this->LastWorldLocation = Parent->GetComponentLocation();
    this->Config = InitConfig;

    // Compute base placements.
    TArray<FVector> BaseComponentLocations;
    if (this->Config.LegPlacementStrategy == ELegIKPlacementStrategy::Circle) {
        float DegreeOffset = 360.0f / this->Config.LegCount;

        FRotator Rotator;
        FVector OffsetVector = FVector(0.0f, this->Config.LegBaseComponentOffset, 0.0f);

        Rotator.Yaw += DegreeOffset / 2.0f;

        for (int i = 0; i < this->Config.LegCount; i++) {
            FVector RestComponentLocation = Rotator.RotateVector(OffsetVector);
            BaseComponentLocations.Push(this->Config.BodyCenterOffset + RestComponentLocation);
            Rotator.Yaw += DegreeOffset;
        }
    }
    else if (this->Config.LegPlacementStrategy == ELegIKPlacementStrategy::Biped) {
        FVector SideOffset = FVector(0.0f, this->Config.LegBaseComponentOffset, 0.0f);

        BaseComponentLocations.Push(this->Config.BodyCenterOffset + SideOffset);
        BaseComponentLocations.Push(this->Config.BodyCenterOffset + (SideOffset * -1.0f));
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

void ULegIKInstance::LegIKInstanceTick(float DeltaTime, USceneComponent* Parent) {
    FVector ParentWorldLocation = Parent->GetComponentLocation();
    FRotator ParentRotation = Parent->GetComponentRotation();

    FVector MoveWorldDelta = ParentWorldLocation - this->LastWorldLocation;
    this->LastWorldLocation = ParentWorldLocation;

    bool CriticalDeltaChange = (MoveWorldDelta - this->LastMoveDelta).Size() > 4.0f; // TODO: Conf.
    this->LastMoveDelta = MoveWorldDelta;
    
    if (CriticalDeltaChange) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, MoveWorldDelta.ToString());
    }

    FLegIKTrackGroup* CriticalImpactedGroup = nullptr;
    bool Stepping = false;
    for (int i = 0; i < this->TrackGroups.Num(); i++) {
        FLegIKTrackGroup* Group = &this->TrackGroups[i];

        for (int j = 0; j < Group->Tracks.Num(); j++) {
            if (Group->Tracks[j].LegIKTrackIsStepping()) {
                Stepping = true;
                CriticalImpactedGroup = Group;
                break;
            }
        }
    }

    if (!Stepping || CriticalDeltaChange) {
        // Compute current base positions and offset tolerance given current travel and
        // find group that needs to step most.
        float CurrentOffsetTolerance = this->Config.RestingOffsetTolerance;
        FVector MoveRetargetDelta;
        if (!MoveWorldDelta.IsZero()) {
            CurrentOffsetTolerance = this->Config.MovingOffsetTolerance;
            MoveRetargetDelta += MoveWorldDelta * this->Config.MoveTargetingCoef;
            MoveRetargetDelta /= FMath::Sqrt(MoveRetargetDelta.Size());
        }

        for (int i = 0; i < this->TrackGroups.Num(); i++) {
            FLegIKTrackGroup* Group = &this->TrackGroups[i];

            Group->CurrentComponentLocations = TArray<FVector>();
            for (int j = 0; j < Group->Tracks.Num(); j++) { 
                FVector NewComponentLocation = (
                    ParentWorldLocation +
                    ParentRotation.RotateVector(Group->BaseComponentLocations[j]) +
                    MoveRetargetDelta
                );

                Group->CurrentComponentLocations.Push(NewComponentLocation);
            }
        }

        FLegIKTrackGroup* StepGroup = nullptr;
        if (CriticalDeltaChange) StepGroup = CriticalImpactedGroup;
        else {
            float MaxOffset = CurrentOffsetTolerance;
            for (int i = 0; i < this->TrackGroups.Num(); i++) {
                FLegIKTrackGroup* CheckGroup = &this->TrackGroups[i];

                float CheckOffset = FMath::Abs((
                    CheckGroup->Tracks[0].LegIKTrackWorldLocation() -
                    CheckGroup->CurrentComponentLocations[0]
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
                StepGroup->Tracks[i].LegIKTrackStepTo(StepGroup->CurrentComponentLocations[i], Parent);
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
    this->Script_IKTargets = NewIKTargets;

    /*
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

    Parent->GetOwner()->SetActorLocation(ActorWorldLocation);
    */
}
