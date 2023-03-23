#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AINavNode.generated.h"

UCLASS()
class TRANSIENT_API AAINavNode : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="AI Nav Graph")
	TArray<AAINavNode*> Neighbors;

public:
	AAINavNode();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
