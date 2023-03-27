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
enum class EUnitAnimMovementState : uint8 {
    None,
    Crouch,
    Jump,
    WalkFwd,
    WalkLeft,
    WalkRight,
    WalkBwd,
    RunFwd,
    RunLeft,
    RunRight,
    RunBwd
};

UENUM(BlueprintType)
enum class EUnitAnimArmsMode : uint8 {
    Empty,
    Mpz,
    Xbow,
    Watercan,
    Keycard,
    RepairTool
};

UENUM(BlueprintType)
enum class EUnitAnimArmsModifier : uint8 {
    None,
    Fire,
    Reload,
    Use,
    Interact
};

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class UUnitAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimMovementState Script_MovementState;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsMode Script_ArmsMode;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsModifier Script_ArmsModifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TimeDilation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TorsoPitch;

public:
	UUnitAnimInstance();
};
