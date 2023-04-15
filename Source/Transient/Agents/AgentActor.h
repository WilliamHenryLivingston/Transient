// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "AgentGroup.h"

#include "AgentActor.generated.h"

UCLASS()
class AAgentActor : public AActor {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category="Agent")
    int FactionID;

    TArray<AAgentGroup*> Groups;

public:
    int AgentFactionID();
    void AgentJoinGroup(AAgentGroup* Group);
    void AgentDistributeAlert(AAgentActor* Target);

    virtual void AgentRealignFactions(int NewFactionID, AAgentGroup* Group);
    virtual void AgentTakeAlert(AAgentActor* Target);
};
