// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "ArmorItem.generated.h"

UCLASS()
class TRANSIENT_API AArmorItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Armor")
	float KineticHealth = 300.0f;
};
