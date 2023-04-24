// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIUnit.h"

#include "AIManager.h"
#include "Actions/AttackAction.h"
#include "Actions/BaseBehavior.h"

void UDetectCallback::Callback() {
    Cast<AAIUnit>(this->AI)->AIPushAttack(this->AgroTarget, true);
}

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
		if (Name.Equals("DetectionSource")) {
            // TODO: DetectionSource shouldn't exist.
            this->AimRootComponent = this->DetectionSourceComponent = Check;
        }
	}

    this->ActionExecutorStack = TArray<CAction*>();
    this->ActionExecutorStack.Push(new CBaseAction(&this->Patrol));

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
    this->AIPushAttack(Target, false);
}

AActor* AAIUnit::AIAgroTarget() {
    AActor* Target = nullptr;

    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        AActor* Check = this->ActionExecutorStack[i]->ActionAttackTarget();

        if (Check != nullptr) Target = Check;
    }

    return Target;
}

void AAIUnit::AIPushAttack(AActor* Target, bool AlertGroup) {
    if (this->FullyPassive || Target == nullptr || !IsValid(Target)) return;

    if (AlertGroup && this->Group != nullptr) {
        this->Group->AIGroupDistributeAlert(Target);
    }

    bool IsDuplicate = false;
    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        if (this->ActionExecutorStack[i]->ActionAttackTarget() == Target) {
            IsDuplicate = true;
            break;
        }
    }

    if (!IsDuplicate) {
        CAction* Attack = new CAttackAction(Target);
        BEHAVIOR_LOG_START(Attack);
        this->ActionExecutorStack.Push(Attack);
    }
}

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->DelegateAnimCallback == nullptr && this->AIAgroTarget() == nullptr) {
        AActor* DetectedTarget = this->AICheckDetection();
        if (DetectedTarget != nullptr) {
            UDetectCallback* Callback = new UDetectCallback();
            Callback->AI = this;
            Callback->AgroTarget = DetectedTarget;

            this->DelegateAnimCallback = Callback;
            this->UnitPlayAnimationOnce(EUnitAnimArmsModifier::Detected, this->DetectionAnim, &AAIUnit::ThenDelegateCallback);
        }
    }

    int StopBeyond = -1;
    CAction* DeferredPush = nullptr;
    for (int i = 0; i < this->ActionExecutorStack.Num(); i++) {
        CAction* Executor = this->ActionExecutorStack[i];

        if (i == this->ActionExecutorStack.Num() - 1) {
            FActionTickResult Result = Executor->ActionTick(this, DeltaTime);

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
            FActionParentTickResult Result = Executor->ActionParentTick(this, DeltaTime);

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
            BEHAVIOR_LOG("rollback");
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
    if (this->UnitArmsOccupied()) return;

    AWeaponItem* CurrentWeapon = this->UnitActiveWeapon();
    if (CurrentWeapon == nullptr) return;

    this->OverrideArmsState = true;

    while (this->UnitGetSlotsContainingMagazines(CurrentWeapon->AmmoTypeID).Num() < 2) {
        TSubclassOf<AMagazineItem> AutoSpawn = this->AutoSpawnMagazine;
        if (AutoSpawn == nullptr) AutoSpawn = CurrentWeapon->AutoSpawnMagazine;

        AMagazineItem* Spawned = this->GetWorld()->SpawnActor<AMagazineItem>(
            AutoSpawn,
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

void AAIUnit::DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    if (Source != nullptr) {
        AUnitPawn* AsUnit = Cast<AUnitPawn>(Source);

        AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());

        if (AsUnit != nullptr && Manager->AIIsFactionEnemy(this->FactionID, AsUnit->FactionID, true)) {
            this->AIPushAttack(Source, true);
        }
    }

    Profile.Energy *= 3.0f; // TODO: Better number.
    Super::DamagableTakeDamage_Implementation(Profile, Cause, Source);
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
