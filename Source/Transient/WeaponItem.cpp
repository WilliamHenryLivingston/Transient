#include "WeaponItem.h"

#include "ProjectileActor.h"
#include "MagazineItem.h"

#include "Components/BoxComponent.h"

AWeaponItem::AWeaponItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->InventoryType = EItemInventoryType::Weapon;
	this->Equippable = true;
}

void AWeaponItem::BeginPlay() {
	Super::BeginPlay();
	
	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("MuzzlePosition")) this->MuzzlePosition = Check;
		else if (Name.Equals("ActiveMagazineHost")) this->ActiveMagazineHost = Check;
	}
}

void AWeaponItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeaponItem::WeaponGetTriggerPulled() {
	return this->TriggerPulled;
}

void AWeaponItem::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	this->TriggerPulled = NewTriggerPulled;
}

FVector AWeaponItem::WeaponGetMuzzlePosition() {
	return this->MuzzlePosition->GetComponentLocation();
}

FRotator AWeaponItem::WeaponGetMuzzleRotation() {
	return this->MuzzlePosition->GetComponentRotation();
}

AMagazineItem* AWeaponItem::WeaponGetMagazine() {
	return this->ActiveMagazine;
}

void AWeaponItem::WeaponSwapMagazines(AMagazineItem* NewMagazine) {
	this->WeaponDisposeCurrentMagazine();

	this->ActiveMagazine = NewMagazine;
	if (this->ActiveMagazine == nullptr) return;

	this->CurrentHolder->ItemHolderPlaySound(this->ReloadSound);

	this->ActiveMagazine->AttachToComponent(
		this->ActiveMagazineHost,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);
}

void AWeaponItem::WeaponDisposeCurrentMagazine() {
	if (this->ActiveMagazine == nullptr) return;

	if (this->ActiveMagazine->Ammo > 0) {
		this->ActiveMagazine->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		this->ActiveMagazine->ItemDrop(this->CurrentHolder);
	}
	else {
		this->ActiveMagazine->Destroy();
	}
	this->ActiveMagazine = nullptr;
}

// Stubs.
bool AWeaponItem::WeaponEmpty() { return false; }