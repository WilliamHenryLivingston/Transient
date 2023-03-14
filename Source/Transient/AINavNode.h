#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AINavNode.generated.h"

UCLASS()
class TRANSIENT_API AAINavNode : public AActor {
	GENERATED_BODY()

private:
	static TArray<AAINavNode*> WorldGraph;
	static bool WorldGraphComplete;

public:
	UPROPERTY(EditAnywhere)
	TArray<AAINavNode*> Neighbors;

public:	
	AAINavNode();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	static AAINavNode* AINavGraphNearestNode(FVector TargetPosition);
};
