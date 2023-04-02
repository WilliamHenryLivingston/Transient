// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DoorAnimInstance.generated.h"

UCLASS()
class TRANSIENT_API UDoorAnimInstance : public UAnimInstance {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool Script_Open;
};
