// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Animation/BooleanAnimInstance.h"

#include "DoorActor.generated.h"

UCLASS()
class ADoorActor : public AActor {
	GENERATED_BODY()
	
private:
	UBooleanAnimInstance* Animation;

protected:
	UPROPERTY(EditAnywhere, Category="Door")
	bool Open;
	UPROPERTY(EditAnywhere, Category="Door")
	bool Locked;

public:	
	ADoorActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	bool DoorIsLocked();
	void DoorSetLocked(bool Open);
	void DoorSetOpen(bool Open);
};
