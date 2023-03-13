#pragma once

#include "CoreMinimal.h"

#include "UnitPawn.h"

#include "AIUnit.generated.h"

UCLASS()
class TRANSIENT_API AAIUnit : public AUnitPawn {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	float DetectionDistance = 60.0f;

	UPROPERTY(EditAnywhere)
	AActor* AgroTarget;

	float IdleActionCooldown;

	float IdleTargetYaw;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	AActor* AICheckDetection();
};
