// Copyright: R. Saxifrage, 2023. All rights reserved.

// Agent groups allow sets of agents to coordinate, for example by sharing agro.

#pragma once

#include "CoreMinimal.h"

#include "Transient/MetadataActor.h"

#include "AgentGroup.generated.h"

class AAgentActor;
class AInteractiveAgent;

UCLASS()
class AAgentGroup : public AMetadataActor {
	GENERATED_BODY()

private:
	// Game logic.
	UPROPERTY(EditAnywhere, Category="Agent Group")
	TArray<AAgentActor*> Members;
	UPROPERTY(EditAnywhere, Category="Agent Group")
	TArray<AAgentGroup*> AlertChain;
	UPROPERTY(EditAnywhere, Category="Agent Group")
	TArray<AInteractiveAgent*> Alerters;

public:
	// Game logic.
    UFUNCTION(BlueprintCallable)
	TArray<AInteractiveAgent*> AgentGroupAlerters();

    UFUNCTION(BlueprintCallable)
	void AgentGroupDistributeTarget(AAgentActor* Target);
    UFUNCTION(BlueprintCallable)
	void AgentGroupRemoveMember(AAgentActor* Member);
};
