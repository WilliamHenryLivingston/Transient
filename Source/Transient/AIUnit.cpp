#include "AIUnit.h"

#include "TransientDebug.h"
#include "AIManager.h"
#include "AIActions/AttackAction.h"
#include "AIActions/PatrolAction.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("DetectionSource")) this->DetectionSourceComponent = Check;
	}

    this->ActionExecutorStack = TArray<IAIActionExecutor*>();
    this->ActionExecutorStack.Push(new CPatrolAction(&this->Patrol));

    this->OverrideArmsState = true;
    this->UnitReload();
    this->OverrideArmsState = false;
}

void AAIUnit::EndPlay(EEndPlayReason::Type Reason) {
    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        delete this->ActionExecutorStack[i];
    }

    if (this->Group != nullptr) {
        this->Group->Members.Remove(this);
    }

    AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());
    Manager->AIUnclaimAllNavNodes(this);
    
    Super::EndPlay(Reason);
}

void AAIUnit::AIGroupMemberJoin(AAIGroup* Target) {
    this->Group = Target;
}

void AAIUnit::AIGroupMemberAlert(AActor* Target) {
    if (this->PendingAgroTarget == nullptr && IsValid(Target)) {
        this->PendingAgroTarget = Target;
    }
}

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    AActor* NewAgroTarget = this->PendingAgroTarget;
    this->PendingAgroTarget = nullptr;
    if (NewAgroTarget == nullptr) {
        NewAgroTarget = this->AICheckDetection();
    }

    if (!this->FullyPassive && NewAgroTarget != nullptr && IsValid(NewAgroTarget)) {
        bool IsDuplicate = false;
        for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
            if (this->ActionExecutorStack[i]->AIActionIsAttackOn(NewAgroTarget)) {
                IsDuplicate = true;
                break;
            }
        }

        if (!IsDuplicate) {
            if (this->Group != nullptr) {
                this->Group->AIGroupDistributeAlert(NewAgroTarget);
            }
            this->ActionExecutorStack.Push(new CAttackAction(NewAgroTarget));
        }
    }

    int StopBeyond = -1;
    IAIActionExecutor* DeferredPush = nullptr;
    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        IAIActionExecutor* Executor = this->ActionExecutorStack[i];

        if (i == this->ActionExecutorStack.Num() - 1) {
            FAIActionTickResult Result = Executor->AIActionTick(this, DeltaTime);

            if (Result.Finished) {
                this->ActionExecutorStack.Pop(false);
                delete Executor;
            }
            if (Result.PushChild != nullptr) {
                this->ActionExecutorStack.Push(Result.PushChild);
            }
        }
        else {
            FAIParentActionTickResult Result = Executor->AIParentActionTick(this, DeltaTime);

            if (Result.PushChild != nullptr) {
                DeferredPush = Result.PushChild;
            }
            if (Result.StopChildren) {
                StopBeyond = i;
                break;
            }
        }
    }

    if (StopBeyond >= 0) {
        while (this->ActionExecutorStack.Num() - 1 > StopBeyond) {
            delete this->ActionExecutorStack[this->ActionExecutorStack.Num() - 1];
            this->ActionExecutorStack.Pop(false);
        }
    }
    if (DeferredPush != nullptr) {
        this->ActionExecutorStack.Push(DeferredPush);
    }

    this->UnitPostTick(DeltaTime);
}

void AAIUnit::UnitReload() {
    if (this->UnitAreArmsOccupied()) return;

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
        AUnitPawn* AsUnit = Cast<AUnitPawn>(Source);
        if (AsUnit != nullptr && AsUnit->FactionID != this->FactionID) { // Can agro onto allied factions.
            this->PendingAgroTarget = Source;
        }
    }

    Profile.Energy *= 50.0f; // TODO: Better number.
    Super::UnitTakeDamage(Profile, Source);
}

AActor* AAIUnit::AICheckDetection() {
    FVector HeadLocation = this->DetectionSourceComponent->GetComponentLocation();

    AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());
    
    for (int i = 0; i < 5; i++) {
        FRotator CastRotation = this->GetActorRotation();
        CastRotation.Yaw += 15.0f * (i - 2);

        FHitResult DetectionRayHit;

        FCollisionQueryParams DetectionRayParams;
        DetectionRayParams.AddIgnoredActor(this);

        // ...
        if (!NODEBUG_COLLIDERS) DrawDebugLine(
            this->GetWorld(), 
            HeadLocation,
            HeadLocation + (CastRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f))),
            FColor::Blue,
            false, 
            0.2f,
            100,
            1.0f
        );
        // ...

        this->GetWorld()->LineTraceSingleByChannel(
            DetectionRayHit,
            HeadLocation,
            HeadLocation + (CastRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f))),
            ECollisionChannel::ECC_Visibility, DetectionRayParams
        );

        AActor* AnyHitActor = DetectionRayHit.GetActor();
        if (AnyHitActor == nullptr) continue;

        AUnitPawn* AsPawn = Cast<AUnitPawn>(AnyHitActor);
        if (AsPawn != nullptr && Manager->AIIsFactionEnemy(AsPawn->FactionID, this->FactionID)) {
            return AsPawn;
        }
    }

    return nullptr;
}
