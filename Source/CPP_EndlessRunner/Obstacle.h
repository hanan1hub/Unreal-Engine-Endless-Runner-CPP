// Obstacle.h - Modified for Object Pool System
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

class UStaticMeshComponent;

UCLASS()
class CPP_ENDLESSRUNNER_API AObstacle : public AActor
{
	GENERATED_BODY()

public:
	AObstacle();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UFUNCTION()
	void OnObstacleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	// ===== OBJECT POOL SUPPORT =====
	// This property tracks the obstacle's ID in the object pool
	UPROPERTY()
	int32 PoolObjectID;

	// Helper functions for pool management
	void SetPoolID(int32 ID) { PoolObjectID = ID; }
	int32 GetPoolID() const { return PoolObjectID; }
};