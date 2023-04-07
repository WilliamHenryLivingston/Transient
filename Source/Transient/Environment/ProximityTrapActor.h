// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "../Damagable.h"
#include "../UnitPawn.h"

#include "ProximityTrapActor.generated.h"

UCLASS()
class TRANSIENT_API AProximityTrapActor : public AActor {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	FDamageProfile Damage;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	float ActivationTime;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	int FactionID;
	UPROPERTY(EditAnywhere, Category="Activate Animation")
	float RiseSpeed;

	float ActivationTimer;

	USphereComponent* TriggerComponent;
	UStaticMeshComponent* VisibleComponent;

	AUnitPawn* ActiveTarget;

public:
	AProximityTrapActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnUnitEnterUnchecked(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
	UFUNCTION()
	void OnUnitLeaveUnchecked(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
