// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "UnitAnimInstance.generated.h"

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class UUnitAnimInstance : public UAnimInstance {
    GENERATED_BODY()

// TODO: Privatize?
public:
    UPROPERTY(BlueprintReadWrite)
    EUnitAnimLegsState Script_LegsState;
    UPROPERTY(BlueprintReadWrite)
    EUnitAnimLegsModifier Script_LegsModifier;
    UPROPERTY(BlueprintReadWrite)
    EUnitAnimArmsState Script_ArmsState;
    UPROPERTY(BlueprintReadWrite)
    EUnitAnimArmsModifier Script_ArmsModifier;
    UPROPERTY(BlueprintReadWrite)
    EUnitAnimArmsInteractTarget Script_ArmsInteractTarget;
    UPROPERTY(BlueprintReadWrite)
    float Script_TimeDilation;
    UPROPERTY(BlueprintReadWrite)
    float Script_TorsoPitch;
    UPROPERTY(BlueprintReadWrite)
    float Script_TorsoYaw;

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Script_IKTargets;

private:
	FLegIKProfile Profile;
	TArray<FLegIKTrackGroup> TrackGroups;

    UUnitAgent* ParentUnit;

public:
	UUnitAnimInstance();

public:
    void UnitAnimInit(UUnitAgent* InitParent, FLegIKProfile InitProfile);
	void UnitAnimTick(float DeltaTime, USceneComponent* Parent);

private:
    FLegIKDynamics UnitAnimComputeLegDynamics();
};
