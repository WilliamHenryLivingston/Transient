// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIUnit.h"

#include "AIManager.h"
#include "Actions/AttackAction.h"
#include "Actions/BaseBehavior.h"

//#define DEBUG_DRAWS true

#define BEHAVIOR_LOG_START(S) if (this->DebugBehavior) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, S->DebugInfo);
#define BEHAVIOR_LOG_END(S) if (this->DebugBehavior) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, S->DebugInfo);

void AAIUnit::BeginPlay() {
    for (int i = 0; i < this->ChanceItems.Num(); i++) {
        FChanceItemEntry Entry = this->ChanceItems[i];

        if (FMath::RandRange(0.0f, 1.0f) < Entry.Chance) {
            this->AutoSpawnInitialItems.Push(Entry.Item);
        }
    }

	Super::BeginPlay();

	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("DetectionSource")) this->DetectionSourceComponent = Check;
	}

    this->ActionExecutorStack = TArray<IAIActionExecutor*>();
    this->ActionExecutorStack.Push(new CBaseBehavior(&this->Patrol));

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
                BEHAVIOR_LOG_END(Executor);

                this->ActionExecutorStack.Pop(false);
                delete Executor;
            }
            if (Result.PushChild != nullptr) {
                BEHAVIOR_LOG_START(Result.PushChild);

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
            BEHAVIOR_LOG_END(this->ActionExecutorStack[this->ActionExecutorStack.Num() - 1]);

            delete this->ActionExecutorStack[this->ActionExecutorStack.Num() - 1];
            this->ActionExecutorStack.Pop(false);
        }
    }
    if (DeferredPush != nullptr) {
        BEHAVIOR_LOG_START(DeferredPush);

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

void AAIUnit::DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    if (Source != nullptr) {
        AUnitPawn* AsUnit = Cast<AUnitPawn>(Source);
        if (AsUnit != nullptr && AsUnit->FactionID != this->FactionID) { // Can agro onto allied factions.
            this->PendingAgroTarget = Source;
        }
    }

    Profile.Energy *= 3.0f; // TODO: Better number.
    Super::DamagableTakeDamage(Profile, Cause, Source);
}

AActor* AAIUnit::AICheckDetection() {
    FVector HeadLocation = this->DetectionSourceComponent->GetComponentLocation();

    AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());
    
    for (int i = 0; i < 5; i++) {
        FRotator CastRotation = this->GetActorRotation();
        CastRotation.Yaw += 15.0f * (i - 2);

        #ifdef DEBUG_DRAWS
        DrawDebugLine(
            this->GetWorld(), 
            HeadLocation,
            HeadLocation + (CastRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f))),
            FColor::Blue,
            false, 
            0.2f,
            100,
            1.0f
        );
        #endif

        FHitResult DetectionRayHit;

        FCollisionQueryParams DetectionRayParams;
        DetectionRayParams.AddIgnoredActor(this);

        this->GetWorld()->LineTraceSingleByChannel(
            DetectionRayHit,
            HeadLocation,
            HeadLocation + (CastRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f))),
            ECollisionChannel::ECC_Visibility, DetectionRayParams
        );

        AActor* AnyHitActor = DetectionRayHit.GetActor();
        if (AnyHitActor == nullptr) continue;

        AUnitPawn* AsUnit = Cast<AUnitPawn>(AnyHitActor);
        if (AsUnit != nullptr && Manager->AIShouldDetect(this->FactionID, this->Detection, AsUnit)) {
            return AsUnit;
        }
    }

    return nullptr;
}

void AAIUnit::UnitDie() {
    AMagazineItem* Magazine = Cast<AMagazineItem>(this->UnitGetItemByClass(AMagazineItem::StaticClass()));

    if (Magazine != nullptr) {
        if (FMath::RandRange(0.0f, 1.0f) < 0.5f) {
            this->UnitDropItem(Magazine);
            Magazine->Destroy();
        }
        else {
            Magazine->Ammo = FMath::RandRange(1, Magazine->Capacity);
        }
    }

    Super::UnitDie();
}
