#include "ProjectileActor.h"

#include "UnitPawn.h"

AProjectileActor::AProjectileActor() {
	this->PrimaryActorTick.bCanEverTick = true;
	
	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetCollisionProfileName(FName("OverlapAll"), true);
	this->ColliderComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	this->ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileActor::OnHitTargetUnchecked);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileActor::BeginPlay() {
	Super::BeginPlay();
}

void AProjectileActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector Move = GetActorRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)) * this->Speed * DeltaTime;

	SetActorLocation(GetActorLocation() + Move);
}

void AProjectileActor::OnHitTargetUnchecked(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	AUnitPawn* HitUnit = Cast<AUnitPawn>(OtherActor);

	if (HitUnit != nullptr) {
		HitUnit->UnitTakeDamage(&this->DamageProfile);
	}

	this->Destroy();
}
