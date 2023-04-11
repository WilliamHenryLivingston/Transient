// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StatusUIWidget.generated.h"

UCLASS()
class TRANSIENT_API UStatusUIWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float Script_KineticHealth;
	UPROPERTY(BlueprintReadWrite)
	float Script_ArmorKineticHealth;
	UPROPERTY(BlueprintReadWrite)
	float Script_Energy;
	UPROPERTY(BlueprintReadWrite)
	float Script_Stamina;
};
