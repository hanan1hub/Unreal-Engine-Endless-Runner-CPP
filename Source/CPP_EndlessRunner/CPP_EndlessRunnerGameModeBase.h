// CPP_EndlessRunnerGameModeBase.h - FINAL FIXED VERSION
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_EndlessRunnerGameModeBase.generated.h"

// Forward declarations
class AFloorTile;
class UGameHudWidget;
class ACoin;
class AObstacle;
class UArrowComponent;

// Data Structure Forward Declarations
class FFloorTileQueue;
template<typename T> class FObjectPool;
class FLaneGraph;
class FScoreBST;

// Delegates - MUST be declared BEFORE the class
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinsCountChanged, int32, CoinsCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLivesCountChanged, int32, LivesCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelReset);

/**
 * Game Mode using Data Structures and Algorithms
 */
UCLASS()
class CPP_ENDLESSRUNNER_API ACPP_EndlessRunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// ===== DATA STRUCTURE IMPLEMENTATIONS =====

	// 1. QUEUE: Floor Tile Management (FIFO)
	TSharedPtr<FFloorTileQueue> FloorTileQueue;

	// 2. HASH MAP OBJECT POOLS: Efficient object reuse
	TSharedPtr<FObjectPool<ACoin>> CoinPool;
	TSharedPtr<FObjectPool<AObstacle>> ObstaclePool;

	// 3. GRAPH: Lane system with BFS/DFS
	TSharedPtr<FLaneGraph> LaneGraph;

	// 4. BINARY SEARCH TREE: Score management
	TSharedPtr<FScoreBST> ScoreBST;

	// ===== POOL ID TRACKING =====
	// Track pool IDs for objects (since ObjectPool can't set them directly)
	TMap<AActor*, int32> CoinPoolIDs;
	TMap<AActor*, int32> ObstaclePoolIDs;

	// ===== INITIALIZATION =====

	void InitializeDataStructures();
	void CreateInitialFloorTiles();

	// Object Pool Spawning
	void SpawnItemsUsingPool(AFloorTile* Tile);
	void ReturnPooledObjects(AFloorTile* Tile);

	// Helper to get next pool ID
	int32 GetNextPoolID();

public:
	// ===== FLOOR TILE MANAGEMENT (PUBLIC - Fixed access) =====

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AFloorTile> FloorTileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 NumInitialFloorTiles = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FTransform NextSpawnPoint;

	UFUNCTION()
	const AFloorTile* AddFloorTile(const bool bSpawnItems);

	UFUNCTION()
	void RemoveTile(AFloorTile* Tile);

	// ===== COIN & OBSTACLE CLASSES =====

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<ACoin> CoinClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AObstacle> SmallObstacleClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AObstacle> BigObstacleClass;

	// ===== LANE MANAGEMENT =====

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	TArray<float> LaneSwitchValues;

	// Graph-based lane operations
	UFUNCTION(BlueprintCallable, Category = "Lane System")
	int32 FindOptimalLane(int32 CurrentLane, int32 TargetLane);

	UFUNCTION(BlueprintCallable, Category = "Lane System")
	void SetLaneBlocked(int32 LaneID, bool bBlocked);

	// ===== SCORE MANAGEMENT =====

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 TotalCoins = 0;

	UFUNCTION()
	void AddCoin();

	// Sorting Algorithm: QuickSort
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	void QuickSortScores(TArray<int32>& Scores, int32 Low, int32 High);

	// Searching Algorithm: Binary Search
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	int32 BinarySearchScore(const TArray<int32>& SortedScores, int32 Target);

	// ===== LIVES SYSTEM =====

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxLives = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	int32 CurrentLivesCount;

	UFUNCTION()
	void PlayerDied();

	UFUNCTION()
	void GameOver();

	// ===== UI =====

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameHudWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	UGameHudWidget* GameHud;

	// ===== DELEGATES =====

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCoinsCountChanged OnCoinsCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnLivesCountChanged OnLivesCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnLevelReset OnLevelReset;

private:
	// Helper for QuickSort
	int32 Partition(TArray<int32>& Scores, int32 Low, int32 High);
};