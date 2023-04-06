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
    float StepSwingVerticalOffset;
    UPROPERTY(EditAnywhere)
    float StepReachWorldRadius;
    UPROPERTY(EditAnywhere)
    float LerpRate;
};

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
    FLegIKTrackConfig Config;

private:
    FVector CurrentWorldLocation;

    ELegIKStepPhase StepPhase;
    FVector StepWorldLocation;

public:
    FLegIKTrack();
    FLegIKTrack(FVector InitialWorldLocation, FLegIKTrackConfig Config);
    FVector LegIKTrackWorldLocation();
    bool LegIKTrackIsStepping();
    void LegIKTrackStepTo(FVector WorldOffset, USceneComponent* Parent);
    FVector LegIKTrackTick(float DeltaTime, USceneComponent* Parent);

private:
    FVector GroundHit(FVector Below, USceneComponent* Parent);
};
