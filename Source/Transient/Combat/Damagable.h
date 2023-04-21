// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Damagable.generated.h"

USTRUCT(BlueprintType)
struct FDamageProfile {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Kinetic;
	UPROPERTY(EditAnywhere)
	float Energy;
};

UINTERFACE()
class UDamagable : public UInterface {
	GENERATED_BODY()
};

class IDamagable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source);
};