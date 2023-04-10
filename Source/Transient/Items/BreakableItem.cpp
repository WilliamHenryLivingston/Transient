// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "BreakableItem.h"

#include "Kismet/KismetMathLibrary.h"

void ABreakableItem::DamagableTakeDamage(FDamageProfile Profile, AActor* Source) {
    this->KineticHealth -= Profile.Kinetic;
    if (this->KineticHealth > 0.0f) return;

    this->Equippable = false;

    FVector Location = this->GetActorLocation();

    TArray<UStaticMeshComponent*> Pieces;
    this->GetComponents(Pieces, true);

    this->ColliderComponent->SetSimulatePhysics(false);
    this->ColliderComponent->SetCollisionProfileName(FName("NoCollision"));

    for (int i = 0; i < this->SpawnOnBreak.Num(); i++) {
        this->GetWorld()->SpawnActor<AActor>(
            this->SpawnOnBreak[i],
            this->GetActorLocation(),
            this->GetActorRotation(),
            FActorSpawnParameters()
        );
    }

    for (int i = 0; i < Pieces.Num(); i++) {
        UStaticMeshComponent* Piece = Pieces[i];

        if (Piece == this->VisibleComponent) continue;

        Piece->SetSimulatePhysics(true);
        Piece->SetCollisionProfileName(FName("Item"));
        Piece->AddImpulse(
            UKismetMathLibrary::FindLookAtRotation(Location, Piece->GetCenterOfMass()).RotateVector(
                FVector(this->BreakImpulseStrength, 0.0f, 0.0f)
            ),
            FName("None"), true
        );
    }
}
