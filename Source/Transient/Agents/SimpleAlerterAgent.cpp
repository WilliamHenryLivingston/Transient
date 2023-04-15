// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AlertTower.h"

void ASimpleAlerterAgent::BeginPlay() {
    Super::BeginPlay();

    USkeletalMeshComponent* Skeleton = this->FindComponentByClass<USkeletalMeshComponent>();
    this->Animation = Cast<UBooleanAnimInstance>(Skeleton->GetAnimInstance());
}

void ASimpleAlerterAgent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASimpleAlerterAgent, this->Activated);
}

void ASimpleAlerterAgent::InteractiveUse_Implementation(AUnitAgent* User) {
    this->InteractEnabled = false;

    AAgentManager* Manager = AAgentManager::AgentsGetManager(this->GetWorld());
    if (Manager->AgentsFactionsAreEnemy(this, User)) {
        this->AgentDistributeTarget(User);
    }
    else {
        TArray<AAgentActor*> Targets = this->AgentTargets();
        
        for (int i = 0; i < Targets.Num(); i++) {
            this->AgentDistributeTarget(Targets[i]);
        }
    }
}

void ASimpleAlerterAgent::InteractiveAvailableChanged() {
    Super::InteractiveAvailableChanged();

    this->Animation->Script_Flag = this->Available;
}
