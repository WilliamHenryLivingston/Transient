// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "UsableItem.generated.h"

UCLASS()
class TRANSIENT_API AUsableItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Usable")
	TSubclassOf<AActor> TargetType;
	UPROPERTY(EditAnywhere, Category="Usable")
	bool RequiresTarget;
	UPROPERTY(EditAnywhere, Category="Usable")
	FAnimationConfig UseAnimation;

public:
	virtual void ItemUse(AActor* Target);
};
