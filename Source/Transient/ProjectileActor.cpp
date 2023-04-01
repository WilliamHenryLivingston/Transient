#include "ProjectileActor.h"

#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

#include "UnitPawn.h"

AProjectileActor::AProjectileActor() {
	this->PrimaryActorTick.bCanEverTick = true;
	
	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetCollisionProfileName(FName("OverlapAll"), true);
	this->ColliderComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	this->ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileActor::OnCollideUnchecked);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileActor::BeginPlay() {
	Super::BeginPlay();
}

void AProjectileActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (this->Inertness > 3) return;
	if (this->Inertness > 0) this->Inertness++;

	FVector Move = GetActorRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)) * this->Speed * DeltaTime;

	SetActorLocation(GetActorLocation() + Move);
}

void AProjectileActor::OnCollideUnchecked(
	UPrimitiveComponent* Into,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx,
	bool FromSweep, const FHitResult &Sweep
) {
	if (this->Inertness > 0) return;
	AUnitPawn* HitUnit = Cast<AUnitPawn>(OtherActor);

	if (HitUnit != nullptr) {
		HitUnit->UnitTakeDamage(this->DamageProfile, this->Source);
	
		this->Destroy();
	}
	else if (this->StickOnStaticCollide) {
		this->Inertness = 1;
	}
	else if (this->BulletHoleDecal != nullptr) {
		ADecalActor* Decal = this->GetWorld()->SpawnActor<ADecalActor>(this->GetActorLocation(), FRotator());
		Decal->SetDecalMaterial(this->BulletHoleDecal);
		Decal->SetLifeSpan(30.0f);
		Decal->GetDecal()->DecalSize = FVector(16.0f, 16.0f, 16.0f);
		Decal->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	
		this->Destroy();
	}
	else {
		this->Destroy();
	}
}
