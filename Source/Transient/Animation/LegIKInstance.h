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
struct FLegIKInstanceConfig {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int LegCount;
	UPROPERTY(EditAnywhere)
	ELegIKPlacementStrategy PlacementStrategy;
	UPROPERTY(EditAnywhere)
	float LegRestComponentLocationOffset;
	UPROPERTY(EditAnywhere)
	float TorsoMaxOffset;
	UPROPERTY(EditAnywhere)
	float TorsoMinOffset;
	UPROPERTY(EditAnywhere)
	float TorsoRestOffset;
	UPROPERTY(EditAnywhere)
	FVector CenterOffset;
	UPROPERTY(EditAnywhere)
	float TorsoLerpSpeed;
	UPROPERTY(EditAnywhere)
	TArray<int> LegGroupA;
};

UCLASS()
class TRANSIENT_API ULegIKInstance : public UAnimInstance {
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Script_IKTargets;

private:
	FLegIKInstanceConfig Config;

	TArray<FLegIKTrack> Tracks;
	TArray<FVector> RestComponentLocations;

	FVector LastWorldLocation;
	bool GroupAStepNext;

public:
	void LegIKInstanceInit(FLegIKInstanceConfig InitConfig, FLegIKTrackConfig TracksConfig);
	void LegIKInstanceTick(float DeltaTime, USceneComponent* Parent);	
};
