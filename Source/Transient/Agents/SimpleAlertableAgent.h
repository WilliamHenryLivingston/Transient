// Copyright: R. Saxifrage, 2023. All rights reserved.

// An agent that responds to having targets with a boolean animation state.

// TODO: Into blueprint.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Animation/BooleanAnimInstance.h"

#include "AgentActor.h"

#include "SimpleAlertableAgent.generated.h"

UCLASS()
class ASimpleAlertableAgent : public AAgentActor {
	GENERATED_BODY()

private:
	UBooleanAnimInstance* Animation;

protected:
	virtual void BeginPlay() override;

protected:
	// Cosmetic.
	virtual void AgentHasTargetsChanged_Implementation() override;
};
