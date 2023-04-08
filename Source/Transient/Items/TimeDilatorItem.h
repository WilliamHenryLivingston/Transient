// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "TimeDilatorItem.generated.h"

UCLASS()
class TRANSIENT_API ATimeDilatorItem : public AItemActor {
	GENERATED_BODY()

private:
	USceneComponent* Light;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
