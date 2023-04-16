// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "LegIKTrack.h"
#include "LegIKInstance.h"

#include "LegIKSkeletonComponent.generated.h"

UENUM()
enum class ELegIKProfileType : uint8 {
	Biped
};

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class ULegIKSkeletonComponent : public USkeletalMeshComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Leg IK")
	ELegIKProfileType ProfileType;

	ULegIKInstance* IK;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

protected:
	virtual void BeginPlay() override;

public:
	void LegIKSetDynamics(FLegIKDynamics Dynamics);
};
