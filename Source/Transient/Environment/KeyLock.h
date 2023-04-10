// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Environment/DoorActor.h"

#include "KeyLock.generated.h"

UCLASS()
class TRANSIENT_API AKeyLock : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Keycard")
	int LockID;

	// TODO: Stupid. Trigger system.
	UPROPERTY(EditAnywhere, Category="Use Trigger")
	ADoorActor* OpenOnUse;

public:	
	AKeyLock();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void LockSwipeInvalid();
	virtual void LockSwipeValid();
};
