// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "LegIKInstance.h"

#include "UnitAnimInstance.generated.h"

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class UUnitAnimInstance : public ULegIKInstance {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimLegsState Script_LegsState;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimLegsModifier Script_LegsModifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsState Script_ArmsState;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsModifier Script_ArmsModifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsInteractTarget Script_ArmsInteractTarget;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TimeDilation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TorsoPitch;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TorsoYaw;

public:
	UUnitAnimInstance();
};
