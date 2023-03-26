#pragma once

#include "CoreMinimal.h"

#include "../AINavNode.h"
#include "../ItemActor.h"
#include "AIActionExecutor.h"

#include "PatrolStepAIAction.generated.h"

USTRUCT()
struct FAIPatrolStep {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Travel")
	AAINavNode* Node;
	UPROPERTY(EditAnywhere, Category="Travel")
    float NodeReachDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category="Equip")
	bool EquipItem;
	UPROPERTY(EditAnywhere, Category="Equip")
	TSubclassOf<AItemActor> EquipItemType;

	UPROPERTY(EditAnywhere, Category="Animation")
	int UseItemTimes = -1;
    UPROPERTY(EditAnywhere, Category="Animation")
    AActor* UseItemTarget;

	UPROPERTY(EditAnywhere, Category="Wait")
	float WaitTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category="Wait")
	float WaitTimeMax = 1.0f;
	UPROPERTY(EditAnywhere, Category="Wait")
	bool ScanWhileWaiting = true;
};

class CPatrolStepAIAction : public IAIActionExecutor {
private:
    FAIPatrolStep Step;

    bool TravelStarted;
    bool UseFinished;

    float Timer;
	float ScanTimer;
	FVector ScanVector;
	int RemainingItemUseTimes;

public:
    CPatrolStepAIAction(FAIPatrolStep Step);
    virtual ~CPatrolStepAIAction() override;

public:
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
