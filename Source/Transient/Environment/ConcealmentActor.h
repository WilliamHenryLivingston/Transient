// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "ConcealmentActor.generated.h"

UCLASS()
class AConcealmentActor : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int Score = 1;
	UPROPERTY(EditAnywhere)
	int ScoreCrouched = 2;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnActorEnter(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
	UFUNCTION()
	void OnActorLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
