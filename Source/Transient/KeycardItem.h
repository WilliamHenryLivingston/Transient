// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "UsableItem.h"

#include "KeycardItem.generated.h"

UCLASS()
class TRANSIENT_API AKeycardItem : public AUsableItem {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Keycard")
	int LockID;

public:
	AKeycardItem();

public:
	virtual void ItemUse(AActor* Target) override;
};
