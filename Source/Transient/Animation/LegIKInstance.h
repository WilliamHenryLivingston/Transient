// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "LegIKTrack.h"

#include "LegIKInstance.generated.h"

UENUM(BlueprintType)
enum class ELegIKPlacementStrategy : uint8 {
	Circle,
	Biped
};

USTRUCT()
struct FLegIKTrackGroupConfig {
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
	TArray<int> Members;
};

USTRUCT()
struct FLegIKInstanceConfig {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int LegCount;
	UPROPERTY(EditAnywhere)
	TArray<FLegIKTrackGroupConfig> LegGroups;
	UPROPERTY(EditAnywhere)
	ELegIKPlacementStrategy LegPlacementStrategy;
	UPROPERTY(EditAnywhere)
	float LegBaseComponentOffset;
	UPROPERTY(EditAnywhere)
	float BodyMaxOffset;
	UPROPERTY(EditAnywhere)
	float BodyMinOffset;
	UPROPERTY(EditAnywhere)
	float BodyRestOffset;
	UPROPERTY(EditAnywhere)
	FVector BodyCenterOffset;
	UPROPERTY(EditAnywhere)
	float BodyLerpRate;
	UPROPERTY(EditAnywhere)
	float MoveTargetingCoef;
	UPROPERTY(EditAnywhere)
	float MovingOffsetTolerance;
	UPROPERTY(EditAnywhere)
	float RestingOffsetTolerance;
	UPROPERTY(EditAnywhere)
	float DeltaChangeTolerance;
};

USTRUCT()
struct FLegIKTrackGroup {
	GENERATED_BODY()

public:
	// TODO: Field only exists to avoid *another* struct (TArray limitation).
	TArray<FVector> CurrentWorldLocations;
	TArray<FVector> BaseComponentLocations;
	TArray<FLegIKTrack> Tracks;
};

UCLASS()
class TRANSIENT_API ULegIKInstance : public UAnimInstance {
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Script_IKTargets;

private:
	FLegIKInstanceConfig Config;

	TArray<FLegIKTrackGroup> TrackGroups;

	FVector LastWorldLocation;
	FVector LastMoveDelta;

public:
	void LegIKInstanceInit(USceneComponent* Parent, FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig);
	void LegIKInstanceTick(float DeltaTime, USceneComponent* Parent);	
};
