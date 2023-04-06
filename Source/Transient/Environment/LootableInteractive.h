// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "../Animation/BooleanAnimInstance.h"
#include "InteractiveActor.h"

#include "LootableInteractive.generated.h"

UCLASS()
class TRANSIENT_API ALootableInteractive : public AInteractiveActor {
	GENERATED_BODY()

private:
	UBooleanAnimInstance* Animation;
	UBoxComponent* ClosedStateColliderComponent;
	
public:	
	ALootableInteractive();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void InteractiveUse(AActor* User) override;

};
