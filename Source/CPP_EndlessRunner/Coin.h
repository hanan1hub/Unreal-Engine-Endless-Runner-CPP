// Coin.h - Modified for Object Pool System
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class USoundBase;

UCLASS()
class CPP_ENDLESSRUNNER_API ACoin : public AActor
{
	GENERATED_BODY()

public:
	ACoin();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CoinMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* OverlapSound;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// ===== OBJECT POOL SUPPORT =====
	// This property tracks the coin's ID in the object pool
	UPROPERTY()
	int32 PoolObjectID;

	// Helper functions for pool management
	void SetPoolID(int32 ID) { PoolObjectID = ID; }
	int32 GetPoolID() const { return PoolObjectID; }
};