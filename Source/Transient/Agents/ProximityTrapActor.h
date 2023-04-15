// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "../Damagable.h"
#include "../UnitPawn.h"

#include "ProximityTrapActor.generated.h"

UCLASS()
class TRANSIENT_API AProximityTrapActor : public AActor, public IDamagable {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	FDamageProfile Damage;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	float ActivationTime;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	int KineticHealth = 100.0f;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	int FactionID;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	TSubclassOf<AActor> HitEffect;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	TSubclassOf<AActor> TriggerEffect;
	UPROPERTY(EditAnywhere, Category="Proximity Trap")
	float RiseSpeed;

	float ActivationTimer;

	USphereComponent* TriggerComponent;
	UStaticMeshComponent* VisibleComponent;

	TArray<IDamagable*> ActiveTargets; // TODO: Sphere cast instead.

public:
	AProximityTrapActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) override;

private:
	void TrapDetonate();

	UFUNCTION()
	void OnActorEnter(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
	UFUNCTION()
	void OnActorLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
