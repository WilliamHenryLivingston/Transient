// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "Damagable.h"

#include "ProjectileActor.generated.h"

// TODO: Move.
UCLASS()
class TRANSIENT_API AProjectileActor : public AActor {
	GENERATED_BODY()

public:
	AActor* Source;

private:
	UPROPERTY(EditAnywhere, Category="Projectile")
	float Speed = 500.0f;
	UPROPERTY(EditAnywhere, Category="Projectile")
	FDamageProfile DamageProfile;
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool StickOnStaticCollide;
	UPROPERTY(EditAnywhere, Category="Projectile")
	UMaterial* BulletHoleDecal;

	UPROPERTY(EditDefaultsOnly, Category="BP Object Common")
	UStaticMeshComponent* VisibleComponent;
	UPROPERTY(EditDefaultsOnly, Category="BP Object Common")
	UBoxComponent* ColliderComponent;

	int Inertness;

public:	
	AProjectileActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnCollideUnchecked(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);

};
