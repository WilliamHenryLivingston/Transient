// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UnitAnimInstance.h"

#include "InteractiveActor.generated.h"

UCLASS()
class TRANSIENT_API AInteractiveActor : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Interactive")
	float InteractTime;
	UPROPERTY(EditAnywhere, Category="Interactive")
	EUnitAnimArmsModifier InteractAnimation;

public:
	AInteractiveActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void InteractiveUse(AActor* User);

};
