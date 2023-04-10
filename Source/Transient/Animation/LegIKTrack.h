// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "LegIKProfiles.h"

#include "LegIKTrack.generated.h"

USTRUCT()
struct FLegIKTrack {
    GENERATED_BODY()

// TODO: These public because lazy.
public:
    FLegIKProfile Profile;

    FVector CurrentWorldLocation;
    FVector StepWorldLocation;
    FVector StepStartWorldLocation;

    float StepTimeBudget;

private:
    float StepTime;

public:
    FLegIKTrack();
    FLegIKTrack(FVector InitialWorldLocation, FLegIKProfile Profile);
    float LegIKTrackStepProgress();
    void LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent, float TimeBudget);
    FVector LegIKTrackTick(float DeltaTime, USceneComponent* Parent, FLegIKDynamics Dynamics);

private:
    FVector GroundHit(FVector Below, USceneComponent* Parent);
};

USTRUCT()
struct FLegIKTrackGroup {
	GENERATED_BODY()

public:
	// Field only exists to avoid *another* struct (TArrays don't nest).
	TArray<FVector> CurrentWorldLocations;
	TArray<FVector> BaseComponentLocations;
	TArray<FLegIKTrack> Tracks;
};
