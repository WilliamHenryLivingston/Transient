// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "UnitAnimInstance.generated.h"

USTRUCT()
struct FAnimationConfig {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Animation Config")
	float Time = 1.0f;
};

UENUM(BlueprintType)
enum class EUnitAnimLegsState : uint8 {
    None,
    Crouch,
    Jump,
    WalkFwd,
    WalkLeft,
    WalkRight,
    WalkBwd
};

UENUM(BlueprintType)
enum class EUnitAnimLegsModifier : uint8 {
    None,
    Sprint
};

UENUM(BlueprintType)
enum class EUnitAnimArmsState : uint8 {
    Empty,
    Mpz,
    Xbow,
    Watercan,
    Keycard,
    RepairTool,
    Battery
};

UENUM(BlueprintType)
enum class EUnitAnimArmsModifier : uint8 {
    None,
    Fire,
    Reload,
    Use,
    Interact,
    CheckStatus
};

UENUM(BlueprintType)
enum class EUnitAnimArmsInteractTarget : uint8 {
    None,
    Terminal
};

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class UUnitAnimInstance : public UAnimInstance {
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

public:
	UUnitAnimInstance();
};
