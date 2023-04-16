// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "KeycardItem.generated.h"

UCLASS()
class AKeycardItem : public AItemActor {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Keycard")
	int LockID;

public:
	virtual void ItemUse_Implementation(AActor* Target) override;
};
