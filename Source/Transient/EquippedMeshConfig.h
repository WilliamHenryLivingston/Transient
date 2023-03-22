#pragma once

#include "CoreMinimal.h"

#include "EquippedMeshConfig.generated.h"

USTRUCT()
struct FEquippedMeshConfig {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere)
    FVector Scale;

    UPROPERTY(EditAnywhere)
    FRotator Rotation;
    
    UPROPERTY(EditAnywhere)
    FRotator AltRotation;
};
