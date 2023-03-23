#pragma once

#include "CoreMinimal.h"

#include "UnitPawn.h"
#include "AINavNode.h"

#include "AIUnit.generated.h"

UCLASS()
class TRANSIENT_API AAIUnit : public AUnitPawn {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TSubclassOf<AMagazineItem> AutoSpawnMagazine;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	float DetectionDistance = 60.0f;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TArray<AAINavNode*> Patrol;

	AUnitPawn* AgroTarget;

	float AttackTime;
	int PatrolNext;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	AUnitPawn* AICheckDetection();
};
