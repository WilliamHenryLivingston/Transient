// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AgentGroup.h"

#include "AgentActor.h"

TArray<AAlertTowerAgent*> AAgentGroup::AgentGroupAlerters() {
	return this->Alerters;
}

void AAgentGroup::AgentGroupDistributeTarget(AAgentActor* Target) {
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
