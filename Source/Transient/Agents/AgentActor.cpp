// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentActor.h"

#include "Transient/Common.h"

AAgentActor::AAgentActor() {
    this->SetReplicates(true);
}

void AAgentActor::Destroyed() {
    Super::Destroyed();

    if (this->HasAuthority()) this->AgentClearReferences();
}

void AAgentActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAgentActor, this->FactionID);
}

int AAgentActor::AgentFactionID() const { return this->FactionID; }

bool AAgentActor::AgentHasTargets() const { return this->HasTargets; }

TArray<AAgentActor*> AAgentActor::AgentTargets() const { return this->Targets; }

void AAgentActor::AgentClearReferences() {
    for (int i = 0; i < this->Groups.Num(); i++) {
        this->Groups[i]->AgentGroupRemoveMember(this);
    }
    for (int i = 0; i < this->TargetedBy.Num(); i++) {
        this->TargetedBy[i]->AgentRemoveTarget(this);
    }
    for (int i = 0; i < this->Targets.Num(); i++) {
        this->Targets[i]->TargetedBy.Remove(this);
    }
}

void AAgentActor::AgentJoinGroup(AAgentGroup* Group) {
    this->Groups.Push(Group);
}

void AAgentActor::AgentDistributeTarget(AAgentActor* Target) {
    for (int i = 0; i < this->Groups.Num(); i++) {
        this->Groups[i]->AgentGroupDistributeTarget(Target);
    }
}

void AAgentActor::AgentRealignFactions(int NewFactionID) {
    this->AgentClearReferences();

    this->FactionID = NewFactionID;

    this->Groups = TArray<AAgentGroup*>();
    this->Targets = TArray<AAgentActor*>();
    this->TargetedBy = TArray<AAgentActor*>();
    this->HasTargets = false;
}

void AAgentActor::AgentAddTarget(AAgentActor* Target) {
    if (this->Targets.Contains(this)) return;

    Target->TargetedBy.Push(this);
    this->Targets.Push(Target);

    this->HasTargets = true;
}

void AAgentActor::AgentRemoveTarget(AAgentActor* Target) {
    Target->TargetedBy.Remove(this);
    this->Targets.Remove(Target);

    this->HasTargets = this->Targets.Num() > 0;
}

void AAgentActor::AgentHasTargetsChanged_Implementation() { }
