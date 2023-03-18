#include "AIUnit.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

    this->NavNode = AAINavNode::AINavGraphNearestNode(this->GetActorLocation());
}

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->AgroTarget != nullptr) {
        FVector MoveTowards = this->AgroTarget->GetActorLocation();

        this->UnitSetTriggerPulled((MoveTowards - this->GetActorLocation()).Length() < 600.0f);
        this->UnitFaceTowards(MoveTowards);
        this->UnitMoveTowards(MoveTowards);
    }

    this->AgroTarget = this->AICheckDetection();

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
