#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationData.h"

#include "DebugViewActor.h"
#include "AINavNode.h"

#include "AINavManager.generated.h"

struct FNavNodeClaim {
public:
	AAINavNode* Node;
	AActor* Claimer;
};

UCLASS()
class TRANSIENT_API AAINavManager : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category="Navigation Binding")
	ANavigationData* NavData;

	TArray<AAINavNode*> Nodes;

private:
	TArray<FNavNodeClaim> Claims;

public:	
	AAINavManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	bool NavIsNodeClaimed(AAINavNode* Node);
	void NavClaimNode(AAINavNode* Node, AActor* Claimer);
	void NavUnclaimAllNodes(AActor* Claimer);
	TArray<AAINavNode*> NavGetNearestNodes(AActor* From, int Count);
};
