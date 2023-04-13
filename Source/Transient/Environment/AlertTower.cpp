// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AlertTower.h"

#include "../AI/AIUnit.h"

void AAlertTower::BeginPlay() {
    Super::BeginPlay();

    this->Animation = Cast<UBooleanAnimInstance>(this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
}

void AAlertTower::InteractiveUse(AActor* User) {
    this->Animation->Script_Flag = true;
    this->InteractEnabled = false;

    AActor* Onto = User;
    AAIUnit* AsAI = Cast<AAIUnit>(User);
    if (AsAI != nullptr) Onto = AsAI->AIAgroTarget();
    for (int i = 0; i < this->Groups.Num(); i++) {
        this->Groups[i]->AIGroupDistributeAlert(Onto);
    }

    Super::InteractiveUse(User);
}

void AAlertTower::AIGroupMemberJoin(AAIGroup* Group) {
    this->Groups.Push(Group);
}
