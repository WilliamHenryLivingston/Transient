// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BeamWeapon.h"

#include "Kismet/KismetMathLibrary.h"

void ABeamWeapon::BeginPlay() {
    Super::BeginPlay();

    this->BeamFX = this->FindComponentByClass<UNiagaraComponent>();
    this->BeamFX->SetVisibility(false);
}

void ABeamWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->CurrentHolder == nullptr) this->TriggerPulled = false;

    if (this->TriggerPulled && this->ActiveMagazine != nullptr && this->ActiveMagazine->Ammo > 0) {
        this->BeamFX->SetVisibility(true);

        AActor* HolderActor = Cast<AActor>(this->CurrentHolder);

        FRotator FireRotation = this->CurrentHolder->ItemHolderGetRotation();
        FVector WorldMuzzle = this->WeaponGetMuzzlePosition();
        FVector BeamMax = WorldMuzzle + FireRotation.RotateVector(FVector(this->MaxDistance, 0.0f, 0.0f));

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(HolderActor);

        FHitResult HitResult;
        bool Hit = this->GetWorld()->LineTraceSingleByChannel(
            HitResult,
            WorldMuzzle, BeamMax,
            ECollisionChannel::ECC_Visibility,
            Params
        );

        IDamagable* Victim = Cast<IDamagable>(HitResult.GetActor());
        if (Victim != nullptr) {
            FDamageProfile PITProfile;
            PITProfile.Energy = this->DamageProfile.Energy * DeltaTime;
            PITProfile.Kinetic = this->DamageProfile.Kinetic * DeltaTime;

            if (this->HitEffect != nullptr && FMath::RandRange(0.0f, 1.0f) < 0.3f) {
                this->GetWorld()->SpawnActor<AActor>(
                    this->HitEffect,
                    HitResult.ImpactPoint,
                    UKismetMathLibrary::FindLookAtRotation(HitResult.GetActor()->GetActorLocation(), HitResult.ImpactPoint),
                    FActorSpawnParameters()
                );
            }

            Victim->DamagableTakeDamage(PITProfile, HolderActor);
        }

        FVector BeamEnd = Hit ? HitResult.ImpactPoint : BeamMax;
        this->BeamFX->SetVectorParameter(FName("Beam End"), FireRotation.UnrotateVector(BeamEnd - WorldMuzzle));

        this->ActiveMagazine->Ammo -= this->BurnRate * DeltaTime;
        if (this->ActiveMagazine->Ammo <= 0) {
            this->TriggerPulled = false;
            this->ActiveMagazine->Destroy();
            this->ActiveMagazine = nullptr;
        }
    }
    else {
        this->BeamFX->SetVisibility(false);
        this->TriggerPulled = false;
    }
}
