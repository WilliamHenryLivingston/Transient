#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WeaponActor.generated.h"

USTRUCT()
struct FDamageProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float KineticDamage;
};

UCLASS()
class TRANSIENT_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponActor();

	UPROPERTY(EditAnywhere)
	FDamageProfile DamageProfile;

	UPROPERTY(EditAnywhere)
	bool UseProjectile = true;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* TempProjectileMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VisibleComponent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void WeaponFire();
};
