// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationData.h"

#include "../DebugViewActor.h"
#include "AINavNode.h"

#include "AIManager.generated.h"

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
};

UCLASS()
class TRANSIENT_API AAIManager : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category="Navigation Binding")
	ANavigationData* NavData;
	UPROPERTY(EditAnywhere, Category="Navigation Binding")
	TArray<FFactionAlliance> FactionConfig;

	TArray<AAINavNode*> Nodes;

	UPROPERTY(EditAnywhere, Category="Debug Utils")
	bool DebugSlowMo; // TODO: New actor for stuff like this?
private:
	TArray<FNavNodeClaim> Claims;

public:	
	AAIManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	static AAIManager* AIGetManagerInstance(UWorld* WorldCtx);

	bool AIIsNavNodeClaimed(AAINavNode* Node);
	void AIClaimNavNode(AAINavNode* Node, AActor* Claimer);
	void AIUnclaimAllNavNodes(AActor* Claimer);
	TArray<AAINavNode*> AIGetNavNearestNodes(AActor* From, int Count);
	bool AIIsFactionEnemy(int MyFaction, int OtherFaction);
	bool AIShouldDetect(int FactionID, int Detection, AActor* Target);
};
