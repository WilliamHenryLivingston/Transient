#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"

#include "DebugViewActor.h"
#include "AINavNode.h"

#include "AINavManager.generated.h"

UCLASS()
class TRANSIENT_API AAINavManager : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category="Navigation Binding")
	ANavMeshBoundsVolume* NavBounds;

	TArray<AAINavNode*> Nodes;

public:	
	AAINavManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	TArray<AAINavNode*> NavGetNearestNodes(AActor* From, int Count);
};
