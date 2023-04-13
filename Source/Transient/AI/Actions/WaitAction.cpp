#include "WaitAction.h"

#include "../AIUnit.h"
#include "AIState.h"

CWaitAction::CWaitAction(float Duration, bool InitScanIfIdle) {
    this->Timer = Duration;
    this->ScanIfIdle = InitScanIfIdle;

    this->DebugInfo = FString::Printf(TEXT("wait %.2f"), Duration);
}

CWaitAction::~CWaitAction() {}

FAIActionTickResult CWaitAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    Owner->UnitSetCrouched(false);

    if (this->Timer > 0.0f) {
        this->Timer -= DeltaTime;

        if (this->ScanIfIdle) {
            if (this->ScanTimer > 0.0f) {
                this->ScanTimer -= DeltaTime;
            }
            else {
                FRotator NewRotation;
                NewRotation.Yaw = FMath::RandRange(0.0f, 360.0f);

                this->ScanVector = Owner->GetActorLocation() + NewRotation.RotateVector(FVector(-100.0f, 0.0f, 0.0f));
                this->ScanTimer = FMath::RandRange(3.0f, 5.0f);
            }
            Owner->UnitFaceTowards(this->ScanVector);
        }
        else if (Owner->AIAgroTarget() != nullptr && Owner->AIState.FindOrAdd(STATE_IN_COVER, 0) != 0) {
            Owner->UnitSetCrouched(true);
        }

        return this->Unfinished;
    }

    return this->Finished;
}
