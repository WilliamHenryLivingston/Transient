// Copyright: R. Saxifrage, 2023. All rights reserved.

// An agent that detontates when it receives a target or takes too much damage,
// dealing damage within a radius after a delay.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"

#include "Transient/EffectActor.h"
#include "Transient/Combat/Damagable.h"

#include "AgentActor.h"

#include "ProximityTrapActor.generated.h"

UCLASS()
class TRANSIENT_API ATrapAgent : public AAgentActor, public IDamagable {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Proximity Trap")
	FDamageProfile Damage;
	UPROPERTY(EditDefaultsOnly, Category="Proximity Trap")
	float DetontationTime;
	UPROPERTY(EditDefaultsOnly, Category="Proximity Trap")
	float DetonationRadius;
	UPROPERTY(EditDefaultsOnly, Category="Proximity Trap")
	TSubclassOf<AEffectActor> DetonateEffect;
	UPROPERTY(EditDefaultsOnly, Category="Proximity Trap")
	int KineticHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=ProximityTrapDetonatingChanged)
	bool Detonating;

	// Game logic.
	float DetontationTimer;

public:
	ATrapAgent();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

public:
	// Game logic.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	virtual void AgentAddTarget(AAgentActor* Target) override;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ProximityTrapDetonate();

private:
	// Cosmetic.
	UFUNCTION(BlueprintNativeEvent)
	void ProximityTrapDetonatingChanged();
};
