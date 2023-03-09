#include "WeaponActor.h"
#include "ProjectileActor.h"

#include "Components/BoxComponent.h"

AWeaponActor::AWeaponActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	UBoxComponent* Collider = (UBoxComponent*)RootComponent;

	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	VisibleComponent->SetupAttachment(RootComponent);
	VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentFireCooldown > 0.0f)
	{
		CurrentFireCooldown -= DeltaTime;
	}
}

void AWeaponActor::WeaponFire()
{
	if (CurrentFireCooldown > 0.0f) return;

	if (UseProjectile)
	{
		FActorSpawnParameters SpawnInfo;

		AProjectileActor* Projectile = GetWorld()->SpawnActor<AProjectileActor>(SpawnInfo);
		
		Projectile->SetActorLocation(GetActorLocation() + (GetActorRotation().RotateVector(MuzzleLocation)));
		Projectile->SetActorRotation(GetActorRotation());
		Projectile->DamageProfile = DamageProfile;

		Projectile->VisibleComponent->SetStaticMesh(TempProjectileMesh);
	}
	else
	{

	}

	CurrentFireCooldown = FireCooldown;
}