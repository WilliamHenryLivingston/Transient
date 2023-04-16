// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BreakableItem.h"

#include "Kismet/KismetMathLibrary.h"

void ABreakableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABreakableItem, this->Broken);
}

void ABreakableItem::BreakableBrokenChanged() {
    this->Equippable = false;

    this->ColliderComponent->SetSimulatePhysics(false);
    this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"));

    FVector Location = this->GetActorLocation();

    TArray<UStaticMeshComponent*> Pieces;
    this->GetComponents(Pieces, true);
    for (int i = 0; i < Pieces.Num(); i++) {
        UStaticMeshComponent* Piece = Pieces[i];

        if (Piece == this->VisibleComponent) continue;

        Piece->SetSimulatePhysics(true);
        Piece->SetCollisionProfileName(FName("Item"));

        FRotator RelativeRotation = UKismetMathLibrary::FindLookAtRotation(Location, Piece->GetCenterOfMass());
        Piece->AddImpulse(
            RelativeRotation.RotateVector(FVector(this->BreakImpulseStrength, 0.0f, 0.0f)),
            FName("None"), true
        );
    }
}

void ABreakableItem::DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    this->KineticHealth -= Profile.Kinetic;
    if (this->KineticHealth > 0.0f) return;

    if (this->LootPool != nullptr) {
        ULootPool* LootPoolInstance = NewObject<ULootPool>(this, this->LootPool);

        TArray<TSubclassOf<AItemActor>> Result = LootPoolInstance->LootPoolEvaluate();

        UWorld* World = this->GetWorld();
        FVector Location = this->GetActorLocation();
        FVector Rotation = this->GetActorRotation();
        for (int i = 0; i < Result.Num(); i++) {
            World->SpawnActor<AActor>(
                Result[i], Location, Rotation,
                FActorSpawnParameters()
            );
        }
    }

    this->Broken = true;

    this->BreakableBrokenChanged();
}
