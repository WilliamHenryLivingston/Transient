#include "AIUnit.h"

#include "PlayerUnit.h"

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->AgroTarget != nullptr) {
        FVector TargetLocation = this->AgroTarget->GetActorLocation();

        this->UnitFaceTowards(TargetLocation, DeltaTime);

        float Distance = (TargetLocation - this->GetActorLocation()).Length();
        if (Distance > 300.0f) {
            this->UnitMoveTowards(TargetLocation, DeltaTime);
        }

        this->UnitSetTriggerPulled(Distance < 600.0f);
    }
    else {
        if (this->IdleActionCooldown > 0.0f) {
            this->IdleActionCooldown -= DeltaTime;
        }
        else {
            this->IdleTargetYaw = this->GetActorRotation().Yaw + 10.0f;
            this->IdleActionCooldown = 1.0f;
        }
        
        this->UnitFaceTowards(FRotator(0.0f, this->IdleTargetYaw, 0.0f).RotateVector(FVector(10.0f, 0.0f, 0.0f)), DeltaTime);
        this->AgroTarget = this->AICheckDetection();
    }
}

AActor* AAIUnit::AICheckDetection() {
    FVector CurrentLocation = this->GetActorLocation();
    FVector DetectionRayEnd = CurrentLocation + (this->GetActorRotation().RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f)));

    FHitResult DetectionRayHit;

    this->GetWorld()->DebugDrawTraceTag = "AIDetection";
    FCollisionQueryParams DetectionRayParams = FCollisionQueryParams(FName("AIDetection"));
    DetectionRayParams.AddIgnoredActor(this);

    this->GetWorld()->LineTraceSingleByChannel(
        DetectionRayHit,
        CurrentLocation, DetectionRayEnd,
        ECollisionChannel::ECC_Visibility, DetectionRayParams
    );

    AActor* AnyHitActor = DetectionRayHit.GetActor();

    if (AnyHitActor == nullptr) return nullptr;

    if (Cast<APlayerUnit>(AnyHitActor) != nullptr) return AnyHitActor;

    return nullptr;
}
