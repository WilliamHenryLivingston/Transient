// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "LegIKProfiles.generated.h"

USTRUCT()
struct FLegIKDynamics {
	GENERATED_BODY()

public:
	float StepDistanceCoef = 1.0f;
    float StepVerticalCoef = 1.0f;
	float BodyBaseCoef = 1.0f;
	float LerpRateCoef = 1.0f;
};

// Using this approach because the editor frequently destroys data
// when working with derived Animation Blueprints / Skeletons.
USTRUCT()
struct FLegIKTrackGroupProfile {
	GENERATED_BODY()

public:
	// Field only exists to avoid *another* struct (TArrays don't nest).
	TArray<FVector> BaseComponentLocations;
};

USTRUCT()
struct FLegIKProfile {
    GENERATED_BODY()

public:
    float GroundCastDistance;
    float StepVerticalOffset;
    float StepReachWorldRadius;
    float StepBaseDistance;
	float BodyBaseOffset;
	float MovingOffsetTolerance;
	float RestingOffsetTolerance;
	float LerpRate;

	TArray<FLegIKTrackGroupProfile> TrackGroups;
};

USTRUCT()
struct FBipedLegIKProfile : public FLegIKProfile {
    GENERATED_BODY()

public:
    FBipedLegIKProfile();
};