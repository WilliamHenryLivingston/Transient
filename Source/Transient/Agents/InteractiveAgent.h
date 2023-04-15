// Copyright: R. Saxifrage, 2023. All rights reserved.

// An actor that can be interacted with. Derived from agent as this interaction
// may be conditional on / impact targeting.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Transient/Animation/UnitAnimInstance.h"

#include "AgentGroup.h"
#include "UnitAgent.h"

#include "InteractiveAgent.generated.h"

class AUnitAgent;

UCLASS()
class AInteractiveAgent : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Interactive")
	float InteractTime;
	UPROPERTY(EditDefaultsOnly, Category="Interactive")
	EUnitAnimArmsInteractTarget InteractAnimation;

protected:
	UPROPERTY(ReplicatedUsing=InteractiveAvailableChanged)
	bool Available = true;

private:
	USceneComponent* InteractLookTargetComponent;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

protected:
	// Cosmetic.
	virtual void InteractiveAvailableChanged();

public:
	// Isomorphic.
	UFUNCTION(BlueprintCallable)
	bool InteractiveAvailable();

	// Game logic.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractiveUse(AUnitAgent* User);
};
