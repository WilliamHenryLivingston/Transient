#pragma once

#include "CoreMinimal.h"

#include "UnitPawn.h"

#include "AIUnit.generated.h"

UCLASS()
class TRANSIENT_API AAIUnit : public AUnitPawn {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	AActor* FollowTarget;

public:	
	virtual void Tick(float DeltaTime) override;
};
