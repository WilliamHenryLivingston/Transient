// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "LegIKProfiles.h"

#include "LegIKTrack.generated.h"

enum class ELegIKStepPhase : uint8 {
    None,
	Lift,
	Swing,
    Place
};

USTRUCT()
struct FLegIKTrack {
    GENERATED_BODY()

public:
    FLegIKProfile Profile;

private:
    FVector CurrentWorldLocation;

    ELegIKStepPhase StepPhase;
    FVector StepWorldLocation;

public:
    FLegIKTrack();
    FLegIKTrack(FVector InitialWorldLocation, FLegIKProfile Profile);
    FVector LegIKTrackWorldLocation();
    ELegIKStepPhase LegIKTrackGetStepPhase();
    void LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent);
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
