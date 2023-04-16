// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "LegIKProfiles.h"
#include "LegIKTrack.h"

#include "LegIKInstance.generated.h"

UCLASS()
class ULegIKInstance : public UAnimInstance {
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Script_IKTargets;

private:
	FLegIKProfile Profile;

	TArray<FLegIKTrackGroup> TrackGroups;

	FLegIKDynamics Dynamics;

public:
	void LegIKInstanceInit(USceneComponent* Parent, FLegIKProfile Profile);
	void LegIKInstanceTick(float DeltaTime, USceneComponent* Parent);
	void LegIKInstanceSetDynamics(FLegIKDynamics NewDynamics);	
};
