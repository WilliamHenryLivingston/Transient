// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "DetectorActor.h"

#include "Kismet/KismetMathLibrary.h"

#include "../UnitPawn.h"
#include "../AI/AIManager.h"

ADetectorActor::ADetectorActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void ADetectorActor::BeginPlay() {
	Super::BeginPlay();
	
	this->Animation = Cast<URotationAnimInstance>(
		this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance()
	);
}

void ADetectorActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector Location = this->GetActorLocation();
	FRotator Rotation = this->GetActorRotation();
	float HalfScan = this->ScanAngle / 2.0f;

	// TODO: Clean up.
	float NewLocalYaw = this->CurrentAngle;
	if (this->Target != nullptr && IsValid(this->Target)) {
		NewLocalYaw = (
			UKismetMathLibrary::FindLookAtRotation(Location, this->Target->GetActorLocation()).Yaw -
			Rotation.Yaw
		);
		if (NewLocalYaw > 180.0f) NewLocalYaw -= 360.0f;
	}
	else {
		if (this->CurrentDirection) {
			NewLocalYaw += DeltaTime * this->ScanSpeed;
			if (NewLocalYaw > HalfScan) this->CurrentDirection = false;
		}
		else {
			NewLocalYaw -= DeltaTime * this->ScanSpeed;
			if (NewLocalYaw < -HalfScan) this->CurrentDirection = true;
		}
	}

	if (NewLocalYaw < -HalfScan) NewLocalYaw = -HalfScan;
	if (NewLocalYaw > HalfScan) NewLocalYaw = HalfScan;

	// TODO: Lerp (not trivial).
	this->CurrentAngle = NewLocalYaw;

	FRotator NewLocalRotation = FRotator(0.0f, NewLocalYaw, this->Pitch);
	FRotator NewWorldRotation = Rotation + NewLocalRotation;
	FVector ScanCast = NewWorldRotation.RotateVector(FVector(this->DetectionDistance, 0.0f, 0.0f));
	FVector ScanBase = Location - FVector(0.0f, 0.0f, this->VerticalOffset);

	if (ShowScan) {
		DrawDebugLine(
			this->GetWorld(),
			ScanBase, ScanBase + ScanCast,
			FColor::Blue,
			false, 1.0f, 0, 0.1f
		);
	}

	FHitResult ScanHit;
	this->GetWorld()->LineTraceSingleByChannel(
		ScanHit,
		ScanBase,
		ScanBase + ScanCast,
		ECollisionChannel::ECC_Visibility,
		FCollisionQueryParams()
	);

	AUnitPawn* AsUnit = Cast<AUnitPawn>(ScanHit.GetActor());
	if (AsUnit != nullptr && AsUnit->UnitIsMoving()) {
		AAIManager* Manager = AAIManager::AIGetManagerInstance(this->GetWorld());

		if (Manager->AIIsFactionEnemy(this->FactionID, AsUnit->FactionID)) {
			this->Target = AsUnit;

			if (this->Group != nullptr) this->Group->AIGroupDistributeAlert(this->Target);
		}
	}

	this->Animation->Script_Rotation = NewLocalRotation + FRotator(0.0f, 90.0f, 0.0f);
}

void ADetectorActor::AIGroupMemberJoin(AAIGroup* NewGroup) {
	this->Group = NewGroup;
}

void ADetectorActor::AIGroupMemberAlert(AActor* AgroTarget) { return; }

void ADetectorActor::DamagableTakeDamage(FDamageProfile Profile, AActor* Source) {
	if (this->Group != nullptr) {
		this->Group->AIGroupDistributeAlert(Source);
	}
	this->Target = Source;

	this->EnergyHealth -= Profile.Energy;
	if (this->EnergyHealth <= 0.0f) {
		this->Destroy(); // TODO: Better.
	}
}
