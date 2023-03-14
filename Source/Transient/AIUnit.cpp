#include "AIUnit.h"

void AAIUnit::BeginPlay() {
	Super::BeginPlay();

    this->NavNode = AAINavNode::AINavGraphNearestNode(this->GetActorLocation());
}

void AAIUnit::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FVector MoveTowards;

    if (this->AgroTarget != nullptr) {
        MoveTowards = this->AgroTarget->GetActorLocation();

        this->UnitSetTriggerPulled((MoveTowards - this->GetActorLocation()).Length() < 600.0f);
    }
    else {
        // TODO: Pathfind patrol?
        MoveTowards = this->NavNode->GetActorLocation();

        this->AgroTarget = this->AICheckDetection();
    }
    
    this->UnitFaceTowards(MoveTowards, DeltaTime);
    this->UnitMoveTowards(MoveTowards, DeltaTime);
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
