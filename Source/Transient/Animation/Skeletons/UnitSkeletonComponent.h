// Copyright: R. Saxifrage, 2023. All rights reserved.

// Base component type for unit skeletal meshes.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "Transient/Animation/IK/LegIKInstance.h"

#include "UnitSkeletonComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UUnitSkeletonComponent : public USkeletalMeshComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Leg IK")
	ELegIKProfileType ProfileType;

	// Isomorphic, non-replicated.
	UUnitAgent* ParentUnit;
	ULegIKInstance* LegIK;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;
};
