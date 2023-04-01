// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DebugViewActor.generated.h"

UCLASS()
class TRANSIENT_API ADebugViewActor : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DebugVisibility;

public:	
	ADebugViewActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
