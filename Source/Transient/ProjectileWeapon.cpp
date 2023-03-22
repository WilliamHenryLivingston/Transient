#include "ProjectileWeapon.h"

void AProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->CurrentFireCooldown > 0.0f) {
        this->CurrentFireCooldown -= DeltaTime;
        return;
    }

    // Sanity check.
    if (this->CurrentHolder == nullptr) {
        this->TriggerPulled = false;
    }

    if (this->TriggerPulled && this->CurrentMagazine > 0) {
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

        this->CurrentFireCooldown = this->FireCooldownTime;
        this->CurrentMagazine--;
    }
    else if (this->TriggerPulled) {
        this->TriggerPulled = false;

        this->CurrentHolder->ItemHolderPlaySound(this->EmptySound);
    }
}

void AProjectileWeapon::WeaponSwapMagazines(int NewAmmoCount) {
    this->CurrentHolder->ItemHolderPlaySound(this->ReloadSound);

    this->CurrentMagazine = NewAmmoCount;
}

bool AProjectileWeapon::WeaponEmpty() {
	return this->CurrentMagazine == 0;
}
