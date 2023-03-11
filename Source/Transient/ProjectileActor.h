#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "ProjectileActor.generated.h"

USTRUCT()
struct FDamageProfile {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float KineticDamage;
};

UCLASS()
class TRANSIENT_API AProjectileActor : public AActor {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float Speed = 500.0f;

	UPROPERTY(EditAnywhere)
	FDamageProfile DamageProfile;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VisibleComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;
	
public:	
	AProjectileActor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHitTargetUnchecked(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);

};
