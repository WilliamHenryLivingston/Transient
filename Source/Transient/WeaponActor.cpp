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

}

void AWeaponActor::WeaponFire()
{
	if (UseProjectile)
	{
		FActorSpawnParameters SpawnInfo;

		AProjectileActor* Projectile = GetWorld()->SpawnActor<AProjectileActor>(SpawnInfo);
		
		FVector MuzzleLocation = GetActorLocation() + (GetActorForwardVector() * 10.0f);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, MuzzleLocation.ToString());

		Projectile->SetActorLocation(MuzzleLocation);
		Projectile->SetActorRotation(GetActorRotation());
		Projectile->DamageProfile = DamageProfile;

		Projectile->VisibleComponent->SetStaticMesh(TempProjectileMesh);
	}
	else
	{

	}
}