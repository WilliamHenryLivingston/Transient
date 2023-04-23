// Copyright: R. Saxifrage, 2023. All rights reserved.

// Singleton manager for agent and AI orchestration.

#pragma once

#include "CoreMinimal.h"
#include "NavigationData.h"

#include "Transient/MetadataActor.h"

#include "AI/AINavNode.h"
#include "AI/AIUnit.h"
#include "AgentActor.h"

#include "AgentManager.generated.h"

struct FNavNodeClaim {
public:
	AAINavNode* Node;
	AActor* Claimer;
};

USTRUCT()
struct FFactionAlliance {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int FactionA;
	UPROPERTY(EditAnywhere)
	int FactionB;
	UPROPERTY(EditAnywhere)
	bool IgnoreDamage;
};

UCLASS()
class AAgentManager : public AMetadataActor {
	GENERATED_BODY()

public:
	// Isomorphic.
	UPROPERTY(EditAnywhere, Replicated, Category="Agent Manager")
	TArray<FFactionAlliance> FactionConfig;
	// Game logic.
	UPROPERTY(EditInstanceOnly, Category="Agent Manager")
	ANavigationData* NavData;

	TArray<AAINavNode*> Nodes; // TODO: Spatial partition octree.

private:
	TArray<FNavNodeClaim> Claims; // TODO: Bi-directional map(s).

public:	
	AAgentManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// Isomorphic.
	UFUNCTION(BlueprintCallable)
	static AAgentManager* AgentsGetManager(UWorld* WorldCtx);
	UFUNCTION(BlueprintCallable)
	bool AgentsFactionsAreEnemy(AAgentActor* A, AAgentActor* B, bool GivenDamage);

	// Game logic.
	bool UnitAIIsNavNodeClaimed(AAINavNode* Node) const;
	void UnitAIClaimNavNode(AAINavNode* Node, AAIUnit* Claimer);
	void UnitAIUnclaimAllNavNodesFor(AAIUnit* Claimer);
	TArray<AAINavNode*> UnitAIGetNearestNavNodesFor(AAIUnit* For, int Count);
	AAINavNode* UnitAIGetBestCoverNavNodeFor(AAIUnit* For, AAgentActor* From, int Count, float MaxDistance);
};
