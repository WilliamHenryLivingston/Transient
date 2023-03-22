#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "UnitAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EUnitAnimMovementState : uint8 {
    None,
    Crouch,
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
    Mpz
};

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class UnitAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimMovementState Script_MovementState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    EUnitAnimArmsMode Script_ArmsMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    bool Script_Reloading;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    bool Script_Interacting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TimeDilation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    float Script_TorsoYRotation;
public:
	UnitAnimInstance();
};
