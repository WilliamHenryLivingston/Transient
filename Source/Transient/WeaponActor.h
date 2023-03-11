#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectileActor.h"

#include "WeaponActor.generated.h"

UCLASS()
class TRANSIENT_API AWeaponActor : public AActor {
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VisibleComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;

protected:
	UPROPERTY(EditAnywhere)
	FVector MuzzleLocation;

	bool TriggerPulled;

public:
	AWeaponActor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void WeaponSetTriggerPulled(bool NewTriggerPulled);
};
