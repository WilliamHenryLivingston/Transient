// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Damagable.h"
#include "../AI/AIGroup.h"
#include "../Animation/RotationAnimInstance.h"

#include "DetectorActor.generated.h"

UCLASS()
class TRANSIENT_API ADetectorActor : public AActor, public IAIGroupMember, public IDamagable {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	int Detection;
	UPROPERTY(EditAnywhere)
	float DetectionDistance = 500.0f;
	UPROPERTY(EditAnywhere)
	float ScanSpeed = 15.0f;
	UPROPERTY(EditAnywhere)
	float ScanAngle = 180.0f;
	UPROPERTY(EditAnywhere)
	float Pitch = 45.0f;
	UPROPERTY(EditAnywhere)
	float VerticalOffset = 200.0f;
	UPROPERTY(EditAnywhere)
	float EnergyHealth = 200.0f;
	UPROPERTY(EditAnywhere)
	bool ShowScan;
	UPROPERTY(EditAnywhere)
	int FactionID;

	float CurrentAngle;
	bool CurrentDirection;

	bool Disabled;

	TArray<USceneComponent*> ActiveOnlyComponents;

	AActor* Target;
	
	AAIGroup* Group;
	URotationAnimInstance* Animation;

public:
	ADetectorActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void AIGroupMemberJoin(AAIGroup* Group) override;
	virtual void AIGroupMemberAlert(AActor* AgroTarget) override;

public:
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Source);
};
