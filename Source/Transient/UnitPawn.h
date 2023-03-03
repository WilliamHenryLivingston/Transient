#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "UnitPawn.generated.h"

UCLASS()
class TRANSIENT_API AUnitPawn : public APawn
{
	GENERATED_BODY()

public:
	AUnitPawn();

protected:
	UPROPERTY(EditAnywhere)
	float Speed = 200.0f;

	UPROPERTY(EditAnywhere)
	float StrafeModifier = 0.5f;

	UPROPERTY(EditAnywhere)
	float StrafeConeAngle = 0.9f;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VisibleComponent;

protected:
	virtual void BeginPlay() override;

	void UnitMoveTowards(FVector Target, float DeltaTime);

	void UnitFaceTowards(FVector Target);

	virtual void OnUnitFace(FRotator Rotation);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
