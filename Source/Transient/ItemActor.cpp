#include "ItemActor.h"

TArray<AItemActor*> AItemActor::WorldItems = TArray<AItemActor*>();

AItemActor::AItemActor() {
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::BeginPlay() {
	Super::BeginPlay();
	
	AItemActor::WorldItems.Push(this);
}

void AItemActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AItemActor::ItemEquip(IItemHolder* Target) {
	AItemActor::WorldItems.Remove(this);

	this->CurrentHolder = Target;
	this->SetActorHiddenInGame(true);
}

void AItemActor::ItemDequip(IItemHolder* Target) {
	AItemActor::WorldItems.Push(this);

	this->CurrentHolder = nullptr;
	this->SetActorLocation(Target->ItemHolderGetLocation());
	this->SetActorHiddenInGame(false);
}

TArray<AItemActor*> AItemActor::ItemsGetNearby(FVector Location, float Reach) {
	TArray<AItemActor*> NearbyItems;

	for (int i = 0; i < AItemActor::WorldItems.Num(); i++) {
		AItemActor* Check = AItemActor::WorldItems[i];
		if (Check == nullptr) continue;

		float Distance = (Check->GetActorLocation() - Location).Size();

		if (Distance < Reach) {
			NearbyItems.Push(Check);
		}
	}

	return NearbyItems;
}

void AItemActor::ItemDestroy() {
	AItemActor::WorldItems.Remove(this);

	this->Destroy();
}