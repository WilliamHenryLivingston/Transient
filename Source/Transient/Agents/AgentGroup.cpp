// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentGroup.h"

#include "AgentActor.h"

const TArray<AAlertTowerAgent*>* AAgentGroup::AgentGroupAlerters() const { return this->Alerters; }

int AAgentGroup::AgentGroupMembersCount() const { return this->Members.Num(); }

void AAgentGroup::AgentGroupDistributeTarget(AAgentActor* Target) const {
	for (int i = 0; i < this->Members.Num(); i++) {
		this->Members[i]->AgentAddTarget(Target);
	}

	for (int i = 0; i < this->AlertChain.Num(); i++) {
		this->AlertChain[i]->AgentGroupDistributeTarget(Target);
	}
}

void AAgentGroup::AgentGroupRemoveMember(AAgentActor* Member) {
	this->Members.Remove(Member);
}
