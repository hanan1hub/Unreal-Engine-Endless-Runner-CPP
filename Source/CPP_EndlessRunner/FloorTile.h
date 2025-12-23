// FloorTile.h - Modified for Object Pool System
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h" 
#include "FloorTile.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class ACPP_EndlessRunnerGameModeBase;
class AObstacle;
class ACoin;

UCLASS()
class CPP_ENDLESSRUNNER_API AFloorTile : public AActor
{
	GENERATED_BODY()

public:
	AFloorTile();


protected:
	virtual void BeginPlay() override;

	// ===== COMPONENTS =====

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FloorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* CenterLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* LeftLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* FloorTriggerBox;

	// ===== CONFIG =====

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float DestroyAfterTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnPercent1 = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnPercent2 = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnPercent3 = 0.5f;

	// ===== CLASSES =====

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AObstacle> SmallObstacleClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AObstacle> BigObstacleClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<ACoin> CoinClass;

	// ===== REFERENCES =====

	UPROPERTY()
	ACPP_EndlessRunnerGameModeBase* GameMode;

	FTimerHandle DestroyHandle;

	// ===== OBJECT POOL INTEGRATION =====

	// OLD: ChildActors - directly spawned and destroyed
	// UPROPERTY()
	// TArray<AActor*> ChildActors;

	// NEW: PooledActors - borrowed from object pools
	// These are NOT destroyed when tile is destroyed
	// They're automatically returned to pools by GameMode
	UPROPERTY()
	TArray<AActor*> PooledActors;

public:
	// ===== ACCESSOR FUNCTIONS =====

	// LANE ACCESSORS - For GameMode to access protected components
	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	UArrowComponent* GetLeftLane() const { return LeftLane; }

	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	UArrowComponent* GetCenterLane() const { return CenterLane; }

	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	UArrowComponent* GetRightLane() const { return RightLane; }

	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	UArrowComponent* GetAttachPointComponent() const { return AttachPoint; }

	// POOLED ACTORS ACCESS - For GameMode to manage pooled objects
	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	const TArray<AActor*>& GetPooledActors() const { return PooledActors; }

	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	void AddPooledActor(AActor* Actor) { PooledActors.Add(Actor); }

	UFUNCTION(BlueprintCallable, Category = "Floor Tile")
	void ClearPooledActors() { PooledActors.Empty(); }

	// ===== FUNCTIONS =====

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void DestroyFloorTile();

	// These functions are now handled by GameMode's object pool system
	// Keeping for backward compatibility with Blueprints
	UFUNCTION(BlueprintCallable)
	void SpawnItems();

	UFUNCTION()
	void SpawnLaneItem(const UArrowComponent* Lane, int32& BigObstaclesCount);

	// Get the attach point transform for the next tile
	FORCEINLINE FTransform GetAttachTransform() const
	{
		return AttachPoint ? AttachPoint->GetComponentTransform() : FTransform::Identity;
	}
};