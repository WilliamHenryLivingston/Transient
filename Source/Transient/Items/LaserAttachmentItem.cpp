// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LaserAttachmentItem.h"

#include "Inventory/InventorySlotComponent.h"
#include "Inventory/UnitPawn.h"
#include "WeaponItem.h"

ALaserAttachmentItem::ALaserAttachmentItem() {
    PrimaryActorTick.bCanEverTick = true;
}

void ALaserAttachmentItem::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    AUnitAgent* Holder = nullptr;
    AWeaponItem* ParentWeapon = nullptr;

    if (this->HasAuthority()) {
        bool Active = false;
        UInventorySlotComponent* ParentSlot = this->ItemSlot();
        if (ParentSlot != nullptr) {
            Holder = this->ItemHolderUnit();
            if (Holder != nullptr) {
                ParentWeapon = Holder->UnitActiveWeapon();
                if (ParentWeapon != nullptr) {
                    Active = ParentSlot->GetOwner() == ParentWeapon;
                }
            }
        }

        if (Active) this->UseNiagara->PlayableStart();
        else this->UseNiagara->PlayableStop();
    }

    if (!this->UseNiagara->PlayableState()) return;

    UNiagaraComponent* NiagaraComponent = this->UseNiagara->ReplicatedNiagaraComponent();

    FVector BaseLocation = this->UseNiagara->GetComponentLocation();
    FRotator Rotation = Holder->GetActorRotation();

    FVector LaserMaxLocation = BaseLocation + Rotation.RotateVector(FVector(3000.0f, 0.0f, 0.0f));

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Holder);

    FHitResult HitResult;
    bool Hit = this->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        BaseLocation, LaserMaxLocation,
        ECollisionChannel::ECC_Visibility,
        Params
    );

    FVector LaserEndLocation = Hit ? HitResult.ImpactPoint : LaserMaxLocation;

    FVector LocalLaserVector = (
        Rotation + ParentWeapon->LaserCorrection
    ).UnrotateVector(LaserEndLocation - BaseLocation);
    NiagaraComponent->SetVectorParameter(FName("Beam End"), LocalLaserVector);
}
