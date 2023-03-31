#pragma once

#include "CoreMinimal.h"

#include "UnitPawn.h"
#include "AINavNode.h"
#include "AIGroup.h"
#include "AIActions/AIActionExecutor.h"
#include "AIActions/PatrolStepAction.h"

#include "AIUnit.generated.h"

UCLASS()
class TRANSIENT_API AAIUnit : public AUnitPawn, public IAIGroupMember {
	GENERATED_BODY()
	
public:
	AAIGroup* Group;

	UPROPERTY(EditAnywhere, Category="AI Behavior")
	bool AssaultIfAlone;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	bool FullyPassive;

	USceneComponent* DetectionSourceComponent;

private:
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TSubclassOf<AMagazineItem> AutoSpawnMagazine;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	float DetectionDistance = 60.0f;
	UPROPERTY(EditAnywhere, Category="AI Behavior")
	TArray<FAIPatrolStep> Patrol;

	UPROPERTY(EditAnywhere, Category="AI Behavior")
	AActor* PendingAgroTarget;

	TArray<IAIActionExecutor*> ActionExecutorStack;

	int PatrolStep;

// AActor.
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

// AUnitPawn.
public:
	virtual void UnitTakeDamage(FDamageProfile Profile, AActor* Source) override;
	virtual void UnitReload() override;

// IAIGroupMember.
public:
	virtual void AIGroupMemberJoin(AAIGroup* Group) override;
	virtual void AIGroupMemberAlert(AActor* AgroTarget) override;

// Internals.
private:
	AActor* AICheckDetection();
};
