// Copyright: R. Saxifrage, 2023. All rights reserved.

// Unit animation type definitions.

#pragma once

#include "CoreMinimal.h"

#include "UnitAnimationState.generated.h"

USTRUCT()
struct FAnimationConfig {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Animation Config")
	float Time = 1.0f;
};

// TODO: Simplify to IK, Jump.
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

// TODO: Fundamentally rework this setup so it doesn't require enumerations.
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
    Battery,
    Mtr,
    Crate,
    Beam
};

UENUM(BlueprintType)
enum class EUnitAnimArmsModifier : uint8 {
    None,
    Fire,
    Reload,
    Use,
    Interact,
    CheckStatus,
    Inventory,
    Detected
};

UENUM(BlueprintType)
enum class EUnitAnimArmsInteractTarget : uint8 {
    None,
    Terminal
};
