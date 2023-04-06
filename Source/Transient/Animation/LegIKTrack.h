// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "LegIKTrack.generated.h"

USTRUCT()
struct FLegIKTrackConfig {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    float GroundVerticalOffset;
    UPROPERTY(EditAnywhere)
    float GroundCastDistance;
    UPROPERTY(EditAnywhere)
    float LerpRate;
    UPROPERTY(EditAnywhere)
    float StepDistance;
    UPROPERTY(EditAnywhere)
    float StepSwingVerticalOffset;
    UPROPERTY(EditAnywhere)
    float RestReturnReachCoef;
};

enum class EIKStepPhase : uint8 {
    None,
	Lift,
	Swing,
    Place
};

USTRUCT()
struct FLegIKTrackTickResult {
    GENERATED_BODY()

public:
    FVector NewTarget;
    bool DidStep;
};

USTRUCT()
struct FLegIKTrack {
    GENERATED_BODY()

public:
    FLegIKTrackConfig Config;

    // TODO: Slightly awkward members.
    FVector RestComponentLocation;
    EIKStepPhase StepPhase;
    FVector CurrentWorldLocation;

    bool ReturnToRest;

private:
    FVector CurrentComponentLocation;
    FVector TargetStepWorldLocation;

public:
    FLegIKTrackTickResult LegIKTrackTick(float DeltaTime, USceneComponent* Parent, bool MayStep);

private:
    FVector IKGroundHit(FVector Below, USceneComponent* Parent);
};
