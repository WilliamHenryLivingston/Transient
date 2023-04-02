// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AIGroup.h"
#include "UnitAnimInstance.h"
#include "DoorActor.h"

#include "InteractiveActor.generated.h"

UCLASS()
class TRANSIENT_API AInteractiveActor : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Interactive")
	float InteractTime;
	UPROPERTY(EditAnywhere, Category="Interactive")
	EUnitAnimArmsInteractTarget InteractAnimation;

	// TODO: So dumb!
	UPROPERTY(EditAnywhere, Category="Use Trigger")
	TArray<AAIGroup*> AlertOnUse;
	UPROPERTY(EditAnywhere, Category="Use Trigger")
	ADoorActor* OpenOnUse;

	USceneComponent* InteractLookTargetComponent;

	// Updated by user. < 0 when inactive.
	float InteractTimer = -1.0f;

public:
	AInteractiveActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void InteractiveUse(AActor* User);

};
