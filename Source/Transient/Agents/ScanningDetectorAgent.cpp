// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ScanningDetectorAgent.h"

AScanningDetectorAgent::AScanningDetectorAgent() {
	PrimaryActorTick.bCanEverTick = true;
}

void AScanningDetectorAgent::BeginPlay() {
	Super::BeginPlay();

	USkeletalMeshComponent* Skeleton = this->FindComponentByClass<USkeletalMeshComponent>();
	this->Animation = Cast<URotationAnimInstance>(Skeleton->GetAnimInstance());
}

void AScanningDetectorAgent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AScanningDetectorAgent, this->CurrentRotation);
}

void AScanningDetectorAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!this->HasAuthority()) return;

	if (this->Disabled) {
		this->CurrentRotation = this->DisabledRotation;
		return;
	}

	this->CurrentAlpha += this->ScanSpeed * DeltaTime;
	if (this->CurrentAlpha > 1.0f) {
		this->CurrentAlpha = 0.0f;
		this->CurrentDirection = !this->CurrentDirection;
	}

	FRotator StartRotation = this->CurrentDirection ? this->ScanStartRotation : this->ScanEndRotation;
	FRotator EndRotation = this->CurrentDirection ? this->ScanEndRotation : this->ScanStartRotation;

	this->CurrentRotation = FMath::Lerp(StartRotation, EndRotation, this->CurrentAlpha);
}

void AScanningDetectorAgent::SimpleDetectorRotationChanged() {
	this->Animation->Script_Rotation = this->CurrentRotation;
}

void AScanningDetectorAgent::DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) {
	this->EnergyHealth -= Profile.Energy;

	if (this->EnergyHealth <= 0.0f) {
		this->Disabled = true;
	}
	else if (Source != nullptr) {
		this->AgentDistributeTarget(Source);
		this->Target = Source;
	}
}
