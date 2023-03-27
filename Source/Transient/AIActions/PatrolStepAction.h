#pragma once

#include "CoreMinimal.h"

#include "../AINavNode.h"
#include "../ItemActor.h"
#include "AIActionExecutor.h"

#include "PatrolStepAction.generated.h"

USTRUCT()
struct FAIPatrolStep {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Travel")
	AAINavNode* Node;
	UPROPERTY(EditAnywhere, Category="Travel")
    float NodeReachDistance = 50.0f;

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

class CPatrolStepAction : public IAIActionExecutor {
private:
    FAIPatrolStep Step;

    bool TravelStarted;
    bool UseFinished;

	int RemainingItemUseTimes;

public:
    CPatrolStepAction(FAIPatrolStep Step);
    virtual ~CPatrolStepAction() override;

public:
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime) override;
};
