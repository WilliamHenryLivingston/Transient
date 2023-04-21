// Copyright: R. Saxifrage, 2023. All rights reserved.

// Waits in the current location for given duration, optionally visually scanning
// the area.

#pragma once

#include "CoreMinimal.h"

#include "Action.h"

class CWaitAction : public CAction {
private:
    float Timer;
	float ScanTimer;

    bool ScanIfIdle;

	FVector ScanVector;

public:
    CWaitAction(float Duration, bool ScanIfIdle);
    virtual ~CWaitAction() override;

public:
	virtual FActionTickResult ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) override;
};
