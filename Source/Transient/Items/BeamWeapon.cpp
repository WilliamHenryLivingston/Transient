// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BeamWeapon.h"

#include "Kismet/KismetMathLibrary.h"

ABeamWeapon::ABeamWeapon() {
    PrimaryActorTick.bCanEverTick = true;
}

void ABeamWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABeamWeapon, this->BeamEndLocation);
}

void ABeamWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (this->WeaponEmpty()) this->WeaponSetTriggerPulled(false);

    if (!this->WeaponTriggerPulled()) {
        if (this->HasAuthority()) {
            this->UseNiagara->PlayableStop();
            this->UseSound->PlayableStop();
        }
        return;
    }

    UNiagaraComponent* BeamNiagaraComponent = this->UseNiagara->ReplicatedNiagaraComponent();

    AActor* Holder = this->ItemHolder();
    FRotator FireRotation = Holder->GetActorRotation();
    FVector MuzzleLocation = this->WeaponMuzzleLocation();

    if (!this->HasAuthority()) {
        FVector LocalBeamVector = FireRotation.UnrotateVector(this->BeamEndLocation - MuzzleLocation);
        BeamNiagaraComponent->SetVectorParameter(FName("Beam End"), LocalBeamVector);
        return;
    }

    this->UseNiagara->PlayableStart();
    this->UseSound->PlayableStart();

    FVector BeamMaxLocation = MuzzleLocation + FireRotation.RotateVector(FVector(this->MaxDistance, 0.0f, 0.0f));

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(HolderActor);

    FHitResult HitResult;
    bool Hit = this->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        WorldMuzzle, BeamMaxLocation,
        ECollisionChannel::ECC_Visibility,
        Params
    );

    AActor* HitActor = HitResult.GetActor();
    IDamagable* Victim = Cast<IDamagable>(HitActor);
    if (Victim != nullptr) {
        FDamageProfile ContinuousProfile;
        ContinuousProfile.Energy = this->DamageProfile.Energy * DeltaTime;
        ContinuousProfile.Kinetic = this->DamageProfile.Kinetic * DeltaTime;

        if (this->HitEffect != nullptr && FMath::RandRange(0.0f, 1.0f) < 0.3f) {
            this->GetWorld()->SpawnActor<AActor>(
                this->HitEffect,
                HitResult.ImpactPoint,
                UKismetMathLibrary::FindLookAtRotation(HitActor->GetActorLocation(), HitResult.ImpactPoint),
                FActorSpawnParameters()
            );
        }

        Victim->DamagableTakeDamage(ContinuousProfile, HolderActor, HolderActor);
    }

    this->BeamEndLocation = Hit ? HitResult.ImpactPoint : BeamMaxLocation;
    FVector BeamLocalVector = FireRotation.UnrotateVector(this->BeamEndLocation - MuzzleLocation);
    BeamNiagaraComponent->SetVectorParameter(FName("Beam End"), BeamLocalVector);

    this->ActiveMagazine->MagazineDeplete(this->BurnRate * DeltaTime);
}
