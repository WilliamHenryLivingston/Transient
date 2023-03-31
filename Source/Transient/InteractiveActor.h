#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "InteractiveActor.generated.h"

UCLASS()
class TRANSIENT_API AInteractiveActor : public AActor {
	GENERATED_BODY()
	
public:
	AInteractiveActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void InteractiveUse(AActor* User);

};
