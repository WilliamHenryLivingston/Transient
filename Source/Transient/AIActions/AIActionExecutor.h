#pragma once

#include "CoreMinimal.h"

class IAIActionExecutor {
public:
	virtual bool AIActionTick(AActor* Owner, float DeltaTime);
	virtual ~IAIActionExecutor();
};
