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
        FActorSpawnParameters SpawnInfo;

        FRotator HolderRotation = this->CurrentHolder->ItemHolderGetRotation();

        FVector FullWeaponOffset = this->CurrentHolder->ItemHolderGetWeaponOffset() + this->MuzzleLocation;

        AProjectileActor* Projectile = this->GetWorld()->SpawnActor<AProjectileActor>(
            this->ProjectileType,
            this->CurrentHolder->ItemHolderGetLocation() + (HolderRotation.RotateVector(FullWeaponOffset)),
            HolderRotation,
            SpawnInfo
        );

        this->CurrentFireCooldown = this->FireCooldownTime;
        this->CurrentMagazine--;
    }
}

void AProjectileWeapon::WeaponSwapMagazines(int NewAmmoCount) {
    this->CurrentMagazine = NewAmmoCount;
}

bool AProjectileWeapon::WeaponEmpty() {
	return this->CurrentMagazine == 0;
}
