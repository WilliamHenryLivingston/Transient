#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "UnitAnimInstance.generated.h"

UCLASS(transient, Blueprintable, hideCategories=AnimInstance, BlueprintType)
class UnitAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ScriptAuto)
    bool ScriptAuto_IsMoving;

public:
	UnitAnimInstance();
};
