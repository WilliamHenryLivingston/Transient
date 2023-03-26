#pragma once

#include "CoreMinimal.h"

class IAIActionExecutor;

class FAIActionExecutionResult {

public:
	bool Finished;
	IAIActionExecutor* PushInner;

public:
	FAIActionExecutionResult();
	FAIActionExecutionResult(bool Finished, IAIActionExecutor* PushInner);
};

class IAIActionExecutor {

protected:
	FAIActionExecutionResult Unfinished;
	FAIActionExecutionResult Finished;

public:
	IAIActionExecutor();
	virtual ~IAIActionExecutor();
	// TODO: Forward declare AAIActor?
	virtual FAIActionExecutionResult AIActionTick(AActor* Owner, float DeltaTime);
};
