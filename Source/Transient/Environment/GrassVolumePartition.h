// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "GrassVolumePartition.generated.h"

UCLASS()
class TRANSIENT_API UGrassVolumePartition : public UBoxComponent {
	GENERATED_BODY()

public:
	TArray<USceneComponent*> Blades;
	TArray<AActor*> TrackedActors;

	TArray<float> Yaws;

	bool FullyInert;

public:
	UGrassVolumePartition();
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPawnEnter(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
	UFUNCTION()
	void OnPawnLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
