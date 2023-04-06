// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "LegIKTrack.h"
#include "LegIKInstance.h"

#include "LegIKSkeletonComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRANSIENT_API ULegIKSkeletonComponent : public USkeletalMeshComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Skeleton IK")
	FLegIKInstanceConfig InstanceConfig;
	UPROPERTY(EditAnywhere, Category="Skeleton IK")
	FLegIKTrackConfig TracksConfig;

	ULegIKInstance* IK;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

protected:
	virtual void BeginPlay() override;

public:
	void LegIKSetDynamics(
		float LerpRate, float DynamicMoveTargetingCoef,
		float DynamicBodyBaseOffsetCoef, float DynamicStepVerticalCoef
	);
};
