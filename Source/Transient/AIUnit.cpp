#include "AIUnit.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();
}

// TODO: Re-write.
void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->AgroTarget != nullptr) {
        AWeaponItem* CurrentWeapon = this->UnitGetActiveWeapon();
        if (CurrentWeapon != nullptr && CurrentWeapon->WeaponEmpty()) {
            this->OverrideArmState = true;

            while (this->UnitGetSlotsContainingMagazines(CurrentWeapon->AmmoTypeID).Num() < 2) {
                AMagazineItem* Spawned = this->GetWorld()->SpawnActor<AMagazineItem>(
                    this->AutoSpawnMagazine,
                    this->GetActorLocation(),
                    this->GetActorRotation(),
                    FActorSpawnParameters()
                );
                if (Spawned == nullptr) break;

                this->UnitTakeItem(Spawned);
            }
            this->OverrideArmState = false;

            this->UnitReload();
        }
        else {
            FVector MoveTowards = this->AgroTarget->GetActorLocation();

            float Distance = (MoveTowards - this->GetActorLocation()).Length();

            FVector Forward = this->GetActorForwardVector();
            FVector Move = MoveTowards - this->GetActorLocation();
            float Angle = acos(Move.Dot(Forward) / (Move.Length() * Forward.Length()));
            if (Angle > 0.15f) {
                this->UnitFaceTowards(MoveTowards);
            }

            if (this->AttackTime <= 0.0f && Distance > 300.0f) {
                this->UnitMoveTowards(MoveTowards);
            }
            else if (this->AttackTime <= 0.0f && Distance < 600.0f) {
                this->AttackTime = 2.0f;
            }

            this->UnitSetCrouched(this->AttackTime > 0.0f);
            bool IsCrouched = this->UnitIsCrouched();
            this->UnitSetTriggerPulled(IsCrouched);
            if (IsCrouched) {
                this->AttackTime -= DeltaTime;
            }
        }
    }
    else {
        this->AgroTarget = this->AICheckDetection();

        if (this->Patrol.Num() > 0) {
            FVector NextLocation = this->Patrol[this->PatrolNext]->GetActorLocation();

            this->UnitMoveTowards(NextLocation);
            this->UnitFaceTowards(NextLocation);

            if ((NextLocation - this->GetActorLocation()).Size() < this->PatrolReachDistance) {
                this->PatrolNext++;

                if (this->PatrolNext >= this->Patrol.Num()) {
                    this->PatrolNext = 0;
                }
            }
        }
    }

    this->UnitPostTick(DeltaTime);
}

AUnitPawn* AAIUnit::AICheckDetection() {
    FVector CurrentLocation = this->GetActorLocation();
    
    for (int i = 0; i < 5; i++) {
        FRotator CastRotation = this->GetActorRotation();
        CastRotation.Yaw += 15.0f * (i - 2);
        FVector DetectionRayEnd = CurrentLocation + (CastRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f)));

        FHitResult DetectionRayHit;

        FCollisionQueryParams DetectionRayParams;
        DetectionRayParams.AddIgnoredActor(this);

        this->GetWorld()->LineTraceSingleByChannel(
            DetectionRayHit,
            CurrentLocation, DetectionRayEnd,
            ECollisionChannel::ECC_Visibility, DetectionRayParams
        );

        AActor* AnyHitActor = DetectionRayHit.GetActor();

        if (AnyHitActor == nullptr) continue;

        AUnitPawn* AsPawn = Cast<AUnitPawn>(AnyHitActor);
        if (AsPawn != nullptr) {
            return AsPawn;
        }
    }

    return nullptr;
}
