#pragma once

#include "CoreMinimal.h"

class IAIActionExecutor;

class FAIActionTickResult {

public:
	bool Finished;
	IAIActionExecutor* PushChild;

public:
	FAIActionTickResult();
	FAIActionTickResult(bool Finished, IAIActionExecutor* PushChild);
};

class FAIParentActionTickResult {

public:
	bool StopChildren;
	IAIActionExecutor* PushChild;

public:
	FAIParentActionTickResult();
	FAIParentActionTickResult(bool StopChildren, IAIActionExecutor* PushChild);
};

class IAIActionExecutor {

protected:
	FAIActionTickResult Unfinished;
	FAIActionTickResult Finished;

public:
	IAIActionExecutor();
	virtual ~IAIActionExecutor();
	// TODO: Forward declare AAIActor?
	virtual bool AIActionIsAttackOn(AActor* Target);
	virtual FAIActionTickResult AIActionTick(AActor* Owner, float DeltaTime);
	virtual FAIParentActionTickResult AIParentActionTick(AActor* Owner, float DeltaTime);
};
