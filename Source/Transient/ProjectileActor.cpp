#include "ProjectileActor.h"

#include "UnitPawn.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	UBoxComponent* Collider = (UBoxComponent*)RootComponent;
	Collider->SetCollisionProfileName(FName("OverlapAll"), true);
	Collider->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AProjectileActor::OnHitTargetUnchecked);

	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	VisibleComponent->SetupAttachment(RootComponent);
	VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Move = GetActorRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)) * Speed * DeltaTime;

	SetActorLocation(GetActorLocation() + Move);
}

void AProjectileActor::OnHitTargetUnchecked(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep)
{
	AUnitPawn* HitUnit = Cast<AUnitPawn>(OtherActor);

	if (HitUnit != nullptr)
	{
		HitUnit->UnitTakeDamage(&DamageProfile);
	}

	Destroy();
}
