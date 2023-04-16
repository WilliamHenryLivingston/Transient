// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ProgressUIWidget.generated.h"

UCLASS()
class UProgressUIWidget : public UUserWidget {
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	float Script_Progress;
	UPROPERTY(BlueprintReadWrite)
	bool Script_InProgress;
};
