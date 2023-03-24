#include "AIActionExecutor.h"

#include "../AIUnit.h"

IAIActionExecutor::~IAIActionExecutor() {}

bool IAIActionExecutor::AIActionTick(AActor* Owner, float DeltaTime) {
    return true;
}
