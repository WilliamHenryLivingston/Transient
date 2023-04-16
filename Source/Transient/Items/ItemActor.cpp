// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ItemActor.h"

#include "Transient/Agents/Units/UnitAgent.h"

AItemActor::AItemActor() {
	this->SetReplicates(true);

	this->RootComponent = this->CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollider"));
	this->ColliderComponent = (UBoxComponent*)this->RootComponent;
	this->ColliderComponent->SetSimulatePhysics(true);
	this->ColliderComponent->SetEnableGravity(true);
	this->ColliderComponent->SetCollisionProfileName(FName("Item"), true);

	this->VisibleComponent = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));
	this->VisibleComponent->SetupAttachment(this->RootComponent);
	this->VisibleComponent->SetCollisionProfileName(FName("NoCollision"), true);
}

void AItemActor::BeginPlay() {
	Super::BeginPlay();

	TArray<UReplicatedSoundComponent*> SoundComponents;
	this->GetComponents(SoundComponents, true);
	for (int i = 0; i < SoundComponents.Num(); i++) {
		UReplicatedSoundComponent* Check = SoundComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("UseSound")) this->UseSound = Check;
	}
	
	TArray<UReplicatedNiagaraComponent*> NiagaraComponents;
	this->GetComponents(NiagaraComponents, true);
	for (int i = 0; i < NiagaraComponents.Num(); i++) {
		UReplicatedNiagaraComponent* Check = NiagaraComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("UseNiagara")) this->UseNiagara = Check;
	}
}

void AItemActor::Destroyed() {
	Super::Destroyed();

	if (this->Slot != nullptr) this->Slot->SlotFinallyBindContent(nullptr);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, this->SlotName);
    DOREPLIFETIME(AItemActor, this->Holder);
}

UInventorySlotComponent* AItemActor::ItemSlot() { return this->Slot; }

AActor* AItemActor::ItemHolder() { return this->Holder; }

AUnitAgent* AItemActor::ItemHolderUnit() { return Cast<AUnitAgent>(this->Holder); }

void AItemActor::ItemStageSlotChange(UInventorySlotComponent* NewSlot) {
	if (NewSlot == nullptr) {
		this->SlotName = TEXT("");
		this->Holder = nullptr;
	}
	else {
		this->SlotName = NewSlot->SlotUniqueName();
		this->Holder = NewSlot->GetOwner();
	}

	this->ItemSlotNameChanged(); // Needs to occur on server too.
}

void AItemActor::ItemSlotNameChanged() {
	if (this->SlotName.Len() == 0) {
		FString CollisionProfile = this->WorldCollisionProfile;
		if (CollisionProfile.Len() == 0) CollisionProfile = FString("Item");

		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		this->ColliderComponent->SetCollisionProfileName(FName(*CollisionProfile), true);
		this->ColliderComponent->SetSimulatePhysics(true);
		return;
	}

	UInventoryComponent* Inventory = this->Holder->FindComponentByClass<UInventoryComponent>();
	this->Slot = Inventory->InventoryFindSlotByName(this->SlotName);
	this->Slot->SlotFinallyBindContent(this);

	this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"), true);
	this->ColliderComponent->SetSimulatePhysics(false);
}

FString AItemActor::ItemInfo() {
	FItemInfo Info;
	Info.Details = TEXT("");
	Info.Name = this->ItemName;

	return Info;
}

void AItemActor::ItemStartUse_Implementation() {
	this->UseNiagara->PlayableStart();
	this->UseSound->PlayableStart();
}

void AItemActor::ItemUse_Implementation(AActor* Target) {
	this->UseNiagara->PlayableStop();
	this->UseSound->PlayableStop();
}

/*
void AItemActor::ItemDrop(AActor* Target) {
	this->CurrentHolder = nullptr;

	FRotator ParentRotation = Target->GetActorRotation();
	FVector DropLocation = Target->GetActorLocation() + ParentRotation.RotateVector(FVector(50.0f, 0.0f, 0.0f));
	float DropRadius = 150.0f;
	DropLocation.X += FMath::RandRange(-DropRadius, DropRadius);
	DropLocation.Y += FMath::RandRange(-DropRadius, DropRadius);
	this->SetActorLocation(DropLocation);

	FRotator DropRotation = ParentRotation;
	DropRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	this->SetActorRotation(DropRotation);

	this->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

	FName WorldProfile = FName("Item");
	if (this->WorldCollisionProfile.Len() > 0) {
		WorldProfile = FName(*this->WorldCollisionProfile);
	}
	this->ColliderComponent->SetCollisionProfileName(WorldProfile, true);
	this->ColliderComponent->SetSimulatePhysics(true);
}
*/
