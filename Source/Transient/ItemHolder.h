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
	virtual FVector ItemHolderGetWeaponOffset();

	virtual FVector ItemHolderGetLocation();

	virtual FRotator ItemHolderGetRotation();

	virtual void ItemHolderPlaySound(USoundBase* Target);
};
