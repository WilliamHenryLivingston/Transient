// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "WaitAction.h"

#define SCAN_ANGLE_TIME_MIN 3.0f
#define SCAN_ANGLE_TIME_MAX 5.0f

CWaitAction::CWaitAction(float Duration, bool InitScanIfIdle) {
    this->Timer = Duration;
    this->ScanIfIdle = InitScanIfIdle;

#if DEBUG_ACTIONS
    this->DebugInfo = FString::Printf(TEXT("wait %.2f"), Duration);
#endif
}

CWaitAction::~CWaitAction() {}

FActionTickResult CWaitAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (this->Timer > 0.0f) {
        this->Timer -= DeltaTime;

        if (!this->ScanIfIdle) return FActionTickResult::Unfinished;

        this->ScanTimer -= DeltaTime;

        if (this->ScanTimer <= 0.0f) {
            FRotator NewRotation;
            NewRotation.Yaw = FMath::RandRange(0.0f, 360.0f);

            this->ScanVector = Owner->GetActorLocation() + NewRotation.RotateVector(FVector(1000.0f, 0.0f, 0.0f));
            this->ScanTimer = FMath::RandRange(SCAN_ANGLE_TIME_MIN, SCAN_ANGLE_TIME_MAX);
        }
        Owner->UnitFaceTowards(this->ScanVector);

        return FActionTickResult::Unfinished;
    }

    return FActionTickResult::Finished;
}
