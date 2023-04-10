// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "Damagable.h"

#include "Debug.h"

void IDamagable::DamagableTakeDamage(FDamageProfile Profile, AActor* Source) {
    ERR_LOG(Cast<AActor>(this), "unset damage trigger");
}
