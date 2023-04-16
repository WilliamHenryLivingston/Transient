// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "Damagable.h"

#include "Debug.h"

void IDamagable::DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    ERR_LOG(Cast<AActor>(this), "unset damage trigger");
}
