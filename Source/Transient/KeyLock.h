// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "KeyLock.generated.h"

UCLASS()
class TRANSIENT_API AKeyLock : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Keycard")
	int LockID;

public:	
	AKeyLock();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void LockSwipeInvalid();
	virtual void LockSwipeValid();
};
