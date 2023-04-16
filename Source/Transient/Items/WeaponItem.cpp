// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "WeaponItem.h"

#include "Components/BoxComponent.h"

#include "MagazineItem.h"

void AWeaponItem::BeginPlay() {
	Super::BeginPlay();

	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("MuzzleLocation")) this->MuzzleLocation = Check;
	}

	TArray<UReplicatedSoundComponent*> SoundComponents;
	this->GetComponents(SoundComponents, true);
	for (int i = 0; i < SoundComponents.Num(); i++) {
		UReplicatedSoundComponent* Check = SoundComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("ReloadSound")) this->ReloadSound = Check;
		else if (Name.Equals("EmptySound")) this->EmptySound = Check;
	}

	this->Inventory = this->FindComponentByClass<UInventoryComponent>();
	this->MagazineSlot = this->Inventory->InventoryFindSlotByName(TEXT("MagazineSlot"));
}

void AWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeaponItem, this->TriggerPulled);
}

bool AWeaponItem::WeaponTriggerPulled() { return this->TriggerPulled; }

void AWeaponItem::WeaponTriggerPulledChanged() { }

AMagazineItem* AWeaponItem::WeaponMagazine() {
	return this->MagazineSlot->SlotContent();
}

FVector AWeaponItem::WeaponMuzzleLocation() {
	return this->MuzzleLocation->GetComponentLocation();
}

bool AWeaponItem::WeaponEmpty() {
	AMagazineItem* Magazine = this->WeaponMagazine();
	return Magazine == nullptr || Magazine->MagazineAmmo() == 0;
}

void AWeaponItem::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	if (NewTriggerPulled && this->WeaponEmpty()) {
		this->EmptySound->PlayableStart();
	}

	this->TriggerPulled = NewTriggerPulled;
}

AMagazineItem* AWeaponItem::WeaponSwapMagazines(AMagazineItem* NewMagazine) {
	AMagazineItem* PrevMagazine = this->MagazineSlot->SlotContent();

	this->ReloadSound->PlayableStart();

	this->MagazineSlot->SlotSetContent(NewMagazine);

	return PrevMagazine;
}
