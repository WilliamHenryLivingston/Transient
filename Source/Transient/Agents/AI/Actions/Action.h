// Copyright: R. Saxifrage, 2023. All rights reserved.

// Actions are composable units of behavior which operate as pollable futures.
// Actions are used in a stack, such that the top action in the stack is ticked once per
// game loop tick until it finishes and/or pushes another action on top of itself.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Common.h"
#include "Transient/Agents/Units/UnitAgent.h"
#include "Transient/Agents/AI/AIState.h"

class CAction;

struct FActionTickResult {
	GENERATED_BODY()

public:
	bool Finished;
	CAction* PushChild;

public:
	static FActionTickResult Unfinished;
	static FActionTickResult Finished;
	static FActionTickResult UnfinishedAnd(CAction* PushAction);
	static FActionTickResult FinishedAnd(CAction* PushAction);
	static FActionTickResult Error(int Code);

private:
	FActionTickResult(bool Finished, CAction* PushChild);
};

struct FActionParentTickResult {
	GENERATED_BODY()

public:
	bool StopChildren;
	CAction* PushChild;

public:
	static FActionParentTickResult Continue;
	static FActionParentTickResult Stop;
	static FActionParentTickResult ContinueAnd(CAction* PushAction);
	static FActionParentTickResult StopAnd(CAction* PushAction);
	static FActionParentTickResult Error(int Code);

private:
	FActionParentTickResult(bool StopChildren, CAction* PushChild);
};

class CAction {

#if DEBUG_AI
public:
	FString DebugInfo;
#endif

public:
	virtual ~CAction();
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime);
	virtual FActionParentTickResult ActionParentTick(AUnitAgent* Owner, CAIState* State, float DeltaTime);
};
