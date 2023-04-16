// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "AnimationDebugPawn.generated.h"

UCLASS()
class AAnimationDebugPawn : public APawn {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float Speed = 200.0f;

	FVector MovementInput;

public:
	AAnimationDebugPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	void InputForward(float AxisValue);
	void InputRight(float AxisValue);
};
