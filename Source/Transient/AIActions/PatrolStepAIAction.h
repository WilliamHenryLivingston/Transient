#pragma once

#include "CoreMinimal.h"

#include "../AINavNode.h"
#include "AIActionExecutor.h"

#include "PatrolStepAIAction.generated.h"

USTRUCT()
struct FAIPatrolStep {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol Step")
	AAINavNode* Node;
	UPROPERTY(EditAnywhere, Category="PatrolStep")
    float NodeReachDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category="Patrol Step")
	FString EquipItemName;

	UPROPERTY(EditAnywhere, Category="Patrol Step")
	int PlayMiscAnimation = -1;
    UPROPERTY(EditAnywhere, Category="Patrol Step")
    AActor* OrientMiscAnimationTowards;

	UPROPERTY(EditAnywhere, Category="Patrol Step")
	float WaitTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category="Patrol Step")
	float WaitTimeMax = 1.0f;
};

class CPatrolStepAIAction : public IAIActionExecutor {
private:
    FAIPatrolStep Step;

    bool TravelFinished;
    bool AnimationFinished;

    float Timer;

public:
    CPatrolStepAIAction(FAIPatrolStep Step);
    virtual ~CPatrolStepAIAction() override;

public:
	virtual bool AIActionTick(AActor* Owner, float DeltaTime) override;
};
