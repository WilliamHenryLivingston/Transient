// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetBlueprint.h"

#include "MainUIWidget.generated.h"

UCLASS()
class TRANSIENT_API UMainUIWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString Script_CurrentItemDescriptor;
};