// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ProjectileWeapon.h"

#include "../UnitPawn.h"

void AProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->FireCooldownTimer > 0.0f) {
        this->FireCooldownTimer -= DeltaTime;
        return;
    }

    if (this->CurrentHolder == nullptr) this->TriggerPulled = false;

    if (this->TriggerPulled && this->ActiveMagazine != nullptr && this->ActiveMagazine->Ammo > 0) {
        FVector WorldMuzzle = this->WeaponGetMuzzlePosition();

        float AppliedSpread = this->Spread * this->CurrentHolder->ItemHolderGetSpreadModifier();

        FRotator SpreadRotation = this->CurrentHolder->ItemHolderGetRotation();
        SpreadRotation.Yaw += FMath::RandRange(-AppliedSpread, AppliedSpread);
        SpreadRotation.Roll += FMath::RandRange(-AppliedSpread, AppliedSpread);
        SpreadRotation.Pitch += FMath::RandRange(-AppliedSpread, AppliedSpread);

        this->CurrentHolder->ItemHolderPlaySound(this->ShootSound);

        AProjectileActor* Projectile = this->GetWorld()->SpawnActor<AProjectileActor>(
            this->ActiveMagazine->ProjectileType,
            WorldMuzzle,
            SpreadRotation,
            FActorSpawnParameters()
        );

        AUnitPawn* UnitHolder = Cast<AUnitPawn>(this->CurrentHolder);
        // TODO: no lol.
        AItemActor* Suppressor = UnitHolder->UnitGetItemByName(FString("mtr suppressor"));
        bool Suppressed = (
            Suppressor != nullptr &&
            this->WeaponHasItemEquipped(Suppressor) &&
            UnitHolder->UnitGetConcealmentScore() > 0
        );
        if (Projectile != nullptr && !Suppressed) {
            Projectile->Source = UnitHolder;
        }

        if (this->SemiAutomatic) {
            this->TriggerPulled = false;
        }
        else {   
            this->FireCooldownTimer = this->FireCooldownTime;
        } 
        
        this->ActiveMagazine->Ammo--;
        if (this->ActiveMagazine->Ammo <= 0) {
            this->ActiveMagazine->Destroy();
        }
    }
    else if (this->TriggerPulled) {
        this->TriggerPulled = false;

        this->CurrentHolder->ItemHolderPlaySound(this->EmptySound);
    }
}
