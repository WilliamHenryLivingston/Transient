// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ControlProjectile.h"

#include "AI/AIUnit.h"
#include "UnitPawn.h"

// TODO: Need base factional actor.
void AControlProjectile::ProjectileHitVictim(IDamagable* Victim) {
    AUnitPawn* SourceAsUnit = Cast<AUnitPawn>(this->Source);
    AAIUnit* AsAI = Cast<AAIUnit>(Victim);
    if (AsAI == nullptr || SourceAsUnit == nullptr) return;

    AsAI->FactionID = SourceAsUnit->FactionID;
    if (AsAI->Group != nullptr) {
        AsAI->Group->Members.Remove(AsAI);

        if (AsAI->Group->Members.Num() > 0) {
            AActor* Next = Cast<AActor>(AsAI->Group->Members[0]);
            AsAI->Group = nullptr;
            AsAI->AIGroupMemberAlert(Next);
        }
        else AsAI->Group = nullptr;
    }
}