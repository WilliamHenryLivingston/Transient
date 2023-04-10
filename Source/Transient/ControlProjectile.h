// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ProjectileActor.h"

#include "ControlProjectile.generated.h"

UCLASS()
class AControlProjectile : public AProjectileActor {
    GENERATED_BODY()

protected:
    virtual void ProjectileHitVictim(IDamagable* Victim) override;
};
