#include "AIUnit.h"

#include "AIActions/AttackAIAction.h"
#include "AIActions/PatrolAIAction.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

    this->OverrideArmsState = true;
    for (int i = 0; i < this->AutoSpawnInitialItems.Num(); i++) {
        AItemActor* Spawned = this->GetWorld()->SpawnActor<AItemActor>(
            this->AutoSpawnInitialItems[i],
            this->GetActorLocation(),
            this->GetActorRotation(),
            FActorSpawnParameters()
        );
        if (Spawned == nullptr) continue;

        this->UnitTakeItem(Spawned);
    }
    this->OverrideArmsState = false;

    this->ActionExecutorStack = TArray<IAIActionExecutor*>();
    this->ActionExecutorStack.Push(new CPatrolAIAction(&this->Patrol));
}

void AAIUnit::EndPlay(EEndPlayReason::Type Reason) {
    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        delete this->ActionExecutorStack[i];
    }

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

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    AActor* NewAgroTarget = this->PendingAgroTarget;
    this->PendingAgroTarget = nullptr;
    if (NewAgroTarget == nullptr) {
        NewAgroTarget = this->AICheckDetection();
    }

    if (NewAgroTarget != nullptr && IsValid(NewAgroTarget)) {
        this->AgroTarget = NewAgroTarget;

        if (this->Group != nullptr) {
            this->Group->AIGroupDistributeAlert(this->AgroTarget);
        }

        this->ActionExecutorStack.Push(new CAttackAIAction(this->AgroTarget));
    }

    if (this->ActionExecutorStack.Num() > 0) {
        IAIActionExecutor* CurrentExecutor = this->ActionExecutorStack[this->ActionExecutorStack.Num() - 1];

        FAIActionExecutionResult Result = CurrentExecutor->AIActionTick(this, DeltaTime);
        if (Result.Finished) {
            this->ActionExecutorStack.Pop(false);
            delete CurrentExecutor;
        }
        if (Result.PushInner) {
            this->ActionExecutorStack.Push(Result.PushInner);
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
        if (AsPawn != nullptr && AsPawn->FactionID != this->FactionID) { return AsPawn; }
    }

    return nullptr;
}

AActor* AAIUnit::AIGetAgroTarget() { return this->AgroTarget; }
