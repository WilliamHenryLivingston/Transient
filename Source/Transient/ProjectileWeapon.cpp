#include "ProjectileWeapon.h"

void AProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->CurrentFireCooldown > 0.0f) {
        this->CurrentFireCooldown -= DeltaTime;
        return;
    }

    if (this->TriggerPulled) {
        FActorSpawnParameters SpawnInfo;

        AProjectileActor* Projectile = this->GetWorld()->SpawnActor<AProjectileActor>(this->ProjectileType, SpawnInfo);
        Projectile->SetActorLocation(this->GetActorLocation() + (this->GetActorRotation().RotateVector(this->MuzzleLocation)));
        Projectile->SetActorRotation(this->GetActorRotation());

        this->CurrentFireCooldown = this->FireCooldownTime;
    }
}
