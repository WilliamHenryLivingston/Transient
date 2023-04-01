// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemHolder.generated.h"

UINTERFACE()
class UItemHolder : public UInterface {
	GENERATED_BODY()
};

class IItemHolder {
	GENERATED_BODY()

public:
	virtual FVector ItemHolderGetLocation();
	virtual FRotator ItemHolderGetRotation();
	virtual float ItemHolderGetSpreadModifier();
	virtual void ItemHolderPlaySound(USoundBase* Target);
};
