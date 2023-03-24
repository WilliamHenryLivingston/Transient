#include "AIUnit.h"

#include "AIActions/AttackAIAction.h"
#include "AIActions/PatrolStepAIAction.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

    if (this->Patrol.Num() > 0) {
        this->CurrentActionExecutor = new CPatrolStepAIAction(this->Patrol[this->PatrolStep]);
    }
}

void AAIUnit::EndPlay(EEndPlayReason::Type Reason) {
    if (this->Group != nullptr) {
        this->Group->Members.Remove(this);
    }
    
    Super::EndPlay(Reason);
}

void AAIUnit::AIGroupMemberJoin(AAIGroup* Target) {
    this->Group = Target;
}

void AAIUnit::AIGroupMemberAlert(AActor* Target) {
    this->PendingAgroTarget = Target;
}

// TODO: Re-write.
void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    AActor* NewAgroTarget = this->PendingAgroTarget;
    this->PendingAgroTarget = nullptr;
    if (NewAgroTarget == nullptr) {
        NewAgroTarget = this->AICheckDetection();
    }

    if (NewAgroTarget != nullptr) {
        this->AgroTarget = NewAgroTarget;

        if (this->Group != nullptr) {
            this->Group->AIGroupDistributeAlert(this->AgroTarget);
        }

        delete this->CurrentActionExecutor;
        this->CurrentActionExecutor = new CAttackAIAction(this->AgroTarget);
    }

    if (this->CurrentActionExecutor != nullptr) {
        bool Done = this->CurrentActionExecutor->AIActionTick(this, DeltaTime);

        if (Done) {
            delete this->CurrentActionExecutor;

            if (this->Patrol.Num() > 0) {
                this->PatrolStep = (this->PatrolStep + 1) % this->Patrol.Num();

                this->CurrentActionExecutor = new CPatrolStepAIAction(this->Patrol[this->PatrolStep]);
            }
        }
    }

    this->UnitPostTick(DeltaTime);
}

void AAIUnit::UnitReload() {
    AWeaponItem* CurrentWeapon = this->UnitGetActiveWeapon();
    if (CurrentWeapon == nullptr) return;

    this->OverrideArmsState = true;

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

    this->OverrideArmsState = false;

    Super::UnitReload();
}

void AAIUnit::UnitTakeDamage(FDamageProfile Profile, AActor* Source) {
    if (Source != nullptr) {
        this->PendingAgroTarget = Source;
        if (this->Group != nullptr) {
            this->Group->AIGroupDistributeAlert(Source);
        }
    }

    Super::UnitTakeDamage(Profile, Source);
}

AActor* AAIUnit::AICheckDetection() {
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
