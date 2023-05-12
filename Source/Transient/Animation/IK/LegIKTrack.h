// Copyright: R. Saxifrage, 2023. All rights reserved.

// Leg IK tracks lerp an IK target over time in discreet steps.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Agents/UnitAgent.h"

#include "LegIKTrack.generated.h"

USTRUCT()
struct FLegIKTrack {
    GENERATED_BODY()

private:
    UUnitAgent* ParentUnit;

    FVector BaseLocation;
    FVector CurrentWorldLocation;
    FVector StepWorldLocation;
    FVector StepStartWorldLocation;

    float StepTimeBudget;
    float StepTime;

public:
    FLegIKTrack(FVector InitBaseLocation, UUnitAgent* InitParent);
    FVector TrackTick(float DeltaTime);
    float TrackStepErrorDistance(FVector GivenVelocity);
    FVector TrackCurrentLocation();
    void TrackStepTo(FVector WorldOffset, float TimeBudget);

private:
    FVector TrackGroundHit(FVector Below);
};

USTRUCT()
struct FLegIKTrackGroup {
	GENERATED_BODY()

public:
	// Field exists to avoid *another* struct defn in usage logic (TArrays don't nest).
	TArray<FVector> CurrentWorldLocations;
	TArray<FVector> BaseComponentLocations;
	TArray<FLegIKTrack> Tracks;
};
