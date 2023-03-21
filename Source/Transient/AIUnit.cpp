#include "AIUnit.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

    this->NavNode = AAINavNode::AINavGraphNearestNode(this->GetActorLocation());
}

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->AgroTarget != nullptr) {
        if (this->WeaponItem != nullptr && this->WeaponItem->WeaponEmpty()) {
            this->UnitReload();
        }
        else {
            FVector MoveTowards = this->AgroTarget->GetActorLocation();

            float Distance = (MoveTowards - this->GetActorLocation()).Length();

            this->UnitSetTriggerPulled(Distance < 600.0f);

            FVector Forward = this->GetActorForwardVector();
            FVector Move = MoveTowards - this->GetActorLocation();
            float Angle = acos(Move.Dot(Forward) / (Move.Length() * Forward.Length()));
            if (Angle > 0.25f) {
                this->UnitFaceTowards(MoveTowards);
            }

            if (Distance > 300.0f) {
                this->UnitMoveTowards(MoveTowards);
            }
        }
    }
    else {
        this->AgroTarget = this->AICheckDetection();
    }

    this->UnitPostTick(DeltaTime);
}

AUnitPawn* AAIUnit::AICheckDetection() {
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

    return Cast<AUnitPawn>(AnyHitActor);
}
