// Copyright: R. Saxifrage, 2023. All rights reserved.

// Agents belong to a faction and can be members of an agent group.

#pragma once

#include "CoreMinimal.h"

#include "AgentGroup.h"

#include "AgentActor.generated.h"

UCLASS()
class AAgentActor : public AActor {
    GENERATED_BODY()

protected:
    // Isomorphic.
    UPROPERTY(EditAnywhere, Replicated, Category="Agent")
    int FactionID;
    UPROPERTY(ReplicatedUsing=AgentHasTargetsChanged)
    bool HasTargets;

    // Game logic.
    TArray<AAgentGroup*> Groups;

    TArray<AAgentActor*> Targets;
    TArray<AAgentActor*> TargetedBy; // Tracked to allow target removal on death.

public:
    AAgentActor();
    virtual void Destroyed() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // Game logic.
    void AgentClearReferences();

protected:
    // Cosmetic.
    UFUNCTION(BlueprintNativeEvent)
    virtual void AgentHasTargetsChanged();

public:
    // Isomorphic.
    UFUNCTION(BlueprintCallable)
    int AgentFactionID() const;
    UFUNCTION(BlueprintCallable)
    bool AgentHasTargets() const;

    // Game logic.
    UFUNCTION(BlueprintCallable)
    const TArray<AAgentActor*>* AgentTargets() const;
    UFUNCTION(BlueprintCallable)
    const TArray<AAgentGroup*>* AgentGroups() const;
    UFUNCTION(BlueprintCallable)
    TArray<AInteractiveAgent*> AgentGroupsAlerters() const;
    UFUNCTION(BlueprintCallable)
    int AgentGroupsMembersCount() const;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AgentDistributeTarget(AAgentActor* Target);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AgentRealignFactions(int NewFactionID);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AgentJoinGroup(AAgentGroup* Group);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    virtual void AgentAddTarget(AAgentActor* Target);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AgentRemoveTarget(AAgentActor* Target);
};
