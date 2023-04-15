// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentActor.h"

#include "../Debug.h"

int AAgentActor::AgentFactionID() {
    return this->FactionID;
}

void AAgentActor::AgentJoinGroup(AAgentGroup* Group) {
    this->Groups.Push(Group);
}

void AAgentActor::AgentDistributeAlert(AAgentActor* Target) {
    for (int i = 0; i < this->Groups.Num(); i++) {
        this->Groups[i]->AgentGroupDistributeAlert(Target);
    }
}

void AAgentActor::AgentRealignFactions(int NewFactionID) {
    for (int i = 0; i < this->Groups.Num(); i++) {
        this->Groups[i]->AgentGroupRemove(this);
    }

    this->FactionID = NewFactionID;
    this->Groups = TArray<AAgentGroup*>();
}

void AAgentActor::AgentTakeAlert(AAgentActor* Target) {

}
