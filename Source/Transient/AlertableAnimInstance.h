// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AlertableAnimInstance.generated.h"

UCLASS()
class TRANSIENT_API UAlertableAnimInstance : public UAnimInstance {
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	bool Script_Alerted;
};
