// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BooleanAnimInstance.generated.h"

UCLASS()
class UBooleanAnimInstance : public UAnimInstance {
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	bool Script_Flag;
};
