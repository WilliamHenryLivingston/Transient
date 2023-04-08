// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "GrassVolumePartition.h"

#include "GrassVolumeActor.generated.h"

UCLASS()
class AGrassVolumeActor : public AActor {
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	UBoxComponent* BoundsComponent;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	int NumBlades;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	int NumSpacePartitions;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	UStaticMesh* BladeMesh;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	float AreaSize = 200.0f;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	float HeightScaleMin = 1.0f;
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	float HeightScaleMax = 1.0f;

	TArray<UGrassVolumePartition*> Partitions;

public:
	UPROPERTY(EditAnywhere, Category="Grass Volume")
	float CrushAlpha = 1.0f;

public:
	AGrassVolumeActor();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(CallInEditor, Category="Grass Volume")
	void GenerateBlades();
};
