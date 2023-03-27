#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DebugViewActor.h"

#include "AINavNode.generated.h"

UCLASS()
class TRANSIENT_API AAINavNode : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="AI Nav")
	TArray<AAINavNode*> Neighbors;

	UPROPERTY(EditAnywhere, Category="AI Nav")
	bool CoverPosition;
	UPROPERTY(EditAnywhere, Category="AI Nav")
	bool CrouchFullCover;

public:
	AAINavNode();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
