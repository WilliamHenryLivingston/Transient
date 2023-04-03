#include "ProjectileWeapon.h"

void AProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->FireCooldownTimer > 0.0f) {
        this->FireCooldownTimer -= DeltaTime;
        return;
    }

    // Sanity check.
    if (this->CurrentHolder == nullptr) {
        this->TriggerPulled = false;
    }

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
        if (Projectile != nullptr) {
            Projectile->Source = Cast<AActor>(this->CurrentHolder);
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

bool AProjectileWeapon::WeaponEmpty() {
	return this->ActiveMagazine == nullptr || this->ActiveMagazine->Ammo == 0;
}