// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

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
	UStaticMesh* BladeMesh;

	TArray<UStaticMeshComponent*> Blades;
	TArray<AActor*> TrackedActors;
	TArray<float> Yaws;

public:
	AGrassVolumeActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(CallInEditor, Category="Grass Volume")
	void GenerateBlades();

	UFUNCTION()
	void OnPawnEnter(UPrimitiveComponent* Into, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIdx, bool FromSweep, const FHitResult &Sweep);
	UFUNCTION()
	void OnPawnLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
