// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "RotationAnimInstance.generated.h"

UCLASS()
class URotationAnimInstance : public UAnimInstance {
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FRotator Script_Rotation;
};
