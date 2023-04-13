// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LegIKProfiles.h"

FBipedLegIKProfile::FBipedLegIKProfile() {
    this->GroundCastDistance = 200.0f;
    this->StepVerticalOffset = 25.0f;
    this->StepBaseDistance = 150.0f;
	this->BodyBaseOffset = 105.0f;
	this->MovingOffsetTolerance = 40.0f;
	this->RestingOffsetTolerance = 15.0f;
    this->LerpRate = 25.0f;

    this->TrackGroups = TArray<FLegIKTrackGroupProfile>();
    FVector SideOffset = FVector(0.0f, 30.0f, 0.0f);

    for (int i = 0; i < 2; i++) {
        FLegIKTrackGroupProfile GroupProfile;

        GroupProfile.BaseComponentLocations.Push(SideOffset);
        this->TrackGroups.Push(GroupProfile);

        SideOffset *= -1.0f;
    }
}
