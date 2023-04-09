// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"

#include "../Damagable.h"
#include "WeaponItem.h"

#include "BeamWeapon.generated.h"

UCLASS()
class TRANSIENT_API ABeamWeapon : public AWeaponItem {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	float MaxDistance;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	float BurnRate;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	FDamageProfile DamageProfile;
	UPROPERTY(EditAnywhere, Category="Beam Weapon")
	TSubclassOf<AActor> HitEffect;
	
	UNiagaraComponent* BeamFX;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
