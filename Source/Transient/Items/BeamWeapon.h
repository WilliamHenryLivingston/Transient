// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"

#include "Transient/Combat/Damagable.h"
#include "Transient/Rep/ReplicatedEffectActor.h"

#include "WeaponItem.h"

#include "BeamWeapon.generated.h"

UCLASS()
class ABeamWeapon : public AWeaponItem {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	float MaxDistance;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	float BurnRate;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	FDamageProfile DamageProfile;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	TSubclassOf<AReplicatedEffectActor> HitEffect;

	// Game logic.
	UPROPERTY(Replicated)
	FVector BeamEndLocation; // TODO: Might be excessive traffic.

public:
	ABeamWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
