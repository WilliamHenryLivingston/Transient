// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../UnitPawn.h"
#include "AINavNode.h"
#include "AIGroup.h"
#include "Actions/AIActionExecutor.h"
#include "Actions/AIState.h"

#include "AIUnit.generated.h"

USTRUCT()
struct FChanceItemEntry {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AItemActor> Item;
	UPROPERTY(EditAnywhere)
	float Chance;
};

class UDetectCallback : public UUnitDelegateAnimationCallback {
public:
	AActor* AI;
	AActor* AgroTarget;

public:
	virtual void Callback() override;
};

UCLASS()
class AAIUnit : public AUnitPawn, public IAIGroupMember {
	GENERATED_BODY()
	
public:
	AAIGroup* Group;

	UPROPERTY(EditAnywhere, Category="AI Behavior")
	bool FullyPassive;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TSubclassOf<AItemActor> EnergyHealItem;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TSubclassOf<AItemActor> KineticHealItem;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	float DefaultWorldSearchRadius = 1000.0f;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	float AlertTowerUseChance = 0.25f;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	bool DebugBehavior;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	bool RandomizePatrol;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	FAnimationConfig DetectionAnim;

	TMap<AI_STATE_T, int> AIState;

	USceneComponent* DetectionSourceComponent;

private:
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TArray<FChanceItemEntry> ChanceItems;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TSubclassOf<AMagazineItem> AutoSpawnMagazine;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	float DetectionDistance = 60.0f;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TArray<AAINavNode*> Patrol;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	int Detection;

	TArray<IAIActionExecutor*> ActionExecutorStack;

	int PatrolStep;

	AActor* CurrentDetectionAnimTarget;

// AActor.
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

// AUnitPawn.
public:
	virtual void DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
	virtual void UnitReload() override;
	virtual void UnitDie() override;

// IAIGroupMember.
public:
	virtual void AIGroupMemberJoin(AAIGroup* Group) override;
	virtual void AIGroupMemberAlert(AActor* AgroTarget) override;

// Internals.
public:
	AActor* AIAgroTarget();
	void AIPushAttack(AActor* Target, bool AlertGroup);

private:
	AActor* AICheckDetection();
};
