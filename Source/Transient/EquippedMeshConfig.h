#pragma once

#include "CoreMinimal.h"

#include "EquippedMeshConfig.generated.h"

USTRUCT()
struct FEquippedMeshConfig {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Equipped Mesh")
    UStaticMesh* Mesh;
    UPROPERTY(EditAnywhere, Category="Equipped Mesh")
    FVector Scale;
    UPROPERTY(EditAnywhere, Category="Equipped Mesh")
    FRotator Rotation;
    UPROPERTY(EditAnywhere, Category="Equipped Mesh")
    FRotator AltRotation; // Some items have two potential hosts.
};
