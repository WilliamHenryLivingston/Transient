// Copyright: R. Saxifrage, 2023. All rights reserved.

// An agent that repeatedly scans between keyframe rotations, using a rotational
// animation state. Relies on a detector component attached to the moving bone.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Combat/Damagable.h"
#include "Transient/Animation/RotationAnimInstance.h"

#include "ScanningDetectorAgent.generated.h"

UCLASS()
class AScanningDetectorAgent : public AAgentActor, public IDamagable {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Detection Scanner")
	float ScanSpeed = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category="Detection Scanner")
	float EnergyHealth = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category="Detection Scanner")
	FRotator ScanStartRotation;
	UPROPERTY(EditDefaultsOnly, Category="Detection Scanner")
	FRotator ScanEndRotation;
	UPROPERTY(EditDefaultsOnly, Category="Detection Scanner")
	FRotator DisabledRotation;

	UPROPERTY(ReplicatedUsing=SimpleDetectorRotationChanged)
	FRotator CurrentRotation;

	URotationAnimInstance* Animation;

	// Game logic.
	float CurrentAlpha;
	bool CurrentDirection;
	bool Disabled;

public:
	AScanningDetectorAgent();
	virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	// Cosmetic.
	void SimpleDetectorRotationChanged();

public:
	// Game logic.
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source);
};
