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

    if (this->TriggerPulled && this->CurrentMagazineAmmo > 0) {
        FRotator HolderRotation = this->CurrentHolder->ItemHolderGetRotation();

        FVector FullWeaponOffset = this->CurrentHolder->ItemHolderGetWeaponOffset() + this->MuzzleLocation;

        float AppliedSpread = this->Spread * this->CurrentHolder->ItemHolderGetSpreadModifier();

        FRotator SpreadRotation = HolderRotation;
        SpreadRotation.Yaw += FMath::RandRange(-AppliedSpread, AppliedSpread);
        SpreadRotation.Roll += FMath::RandRange(-AppliedSpread, AppliedSpread);
        SpreadRotation.Pitch += FMath::RandRange(-AppliedSpread, AppliedSpread);

        this->CurrentHolder->ItemHolderPlaySound(this->ShootSound);

        AProjectileActor* Projectile = this->GetWorld()->SpawnActor<AProjectileActor>(
            this->ProjectileType,
            this->CurrentHolder->ItemHolderGetLocation() + (HolderRotation.RotateVector(FullWeaponOffset)),
            SpreadRotation,
            FActorSpawnParameters()
        );
        if (Projectile != nullptr) {
            Projectile->Source = Cast<AActor>(this->CurrentHolder);
        }

        this->FireCooldownTimer = this->FireCooldownTime;
        this->CurrentMagazineAmmo--;
    }
    else if (this->TriggerPulled) {
        this->TriggerPulled = false;

        this->CurrentHolder->ItemHolderPlaySound(this->EmptySound);
    }
}

void AProjectileWeapon::WeaponSwapMagazines(int NewAmmoCount) {
    this->CurrentHolder->ItemHolderPlaySound(this->ReloadSound);

    this->CurrentMagazineAmmo = NewAmmoCount;
}

bool AProjectileWeapon::WeaponEmpty() {
	return this->CurrentMagazineAmmo == 0;
}
