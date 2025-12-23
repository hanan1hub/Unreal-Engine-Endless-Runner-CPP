// CPP_EndlessRunnerGameModeBase.cpp - FINAL FIXED VERSION
#include "CPP_EndlessRunnerGameModeBase.h"
#include "FloorTile.h"
#include "GameHudWidget.h"
#include "Coin.h"
#include "Obstacle.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

// Include our custom data structures
#include "FloorTileQueue.h"
#include "ObjectPool.h"
#include "LaneGraph.h"
#include "ScoreBST.h"

void ACPP_EndlessRunnerGameModeBase::BeginPlay()
{
	// SAFETY CHECK: Make sure all required classes are set
	if (!CoinClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CoinClass is not set in GameMode! Coins will not spawn."));
	}

	if (!SmallObstacleClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SmallObstacleClass is not set in GameMode! Obstacles will not spawn."));
	}

	if (!FloorTileClass)
	{
		UE_LOG(LogTemp, Fatal, TEXT("FloorTileClass is not set in GameMode! Game cannot run."));
		return;
	}

	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHud = Cast<UGameHudWidget>(CreateWidget(GetWorld(), GameHudWidgetClass));
	check(GameHud);
	GameHud->InitHud(this);
	GameHud->AddToViewport();

	CurrentLivesCount = MaxLives;

	// Initialize Data Structures
	InitializeDataStructures();

	CreateInitialFloorTiles();
}

int32 ACPP_EndlessRunnerGameModeBase::GetNextPoolID()
{
	static int32 NextID = 1; // Start from 1, 0 is reserved for invalid
	return NextID++;
}

void ACPP_EndlessRunnerGameModeBase::InitializeDataStructures()
{
	UE_LOG(LogTemp, Warning, TEXT("=== Initializing Data Structures ==="));

	// 1. Initialize Queue for Floor Tiles
	FloorTileQueue = MakeShared<FFloorTileQueue>();
	UE_LOG(LogTemp, Warning, TEXT("FloorTileQueue initialized"));

	// 2. Initialize Object Pools (Hash Map based)
	if (CoinClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Initializing Coin Pool with class: %s"), *CoinClass->GetName());
		CoinPool = MakeShared<FObjectPool<ACoin>>();
		CoinPool->Initialize(GetWorld(), CoinClass, 20); // Pre-allocate 20 coins
		UE_LOG(LogTemp, Warning, TEXT("Coin Pool initialized with 20 coins"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CoinClass is NULL! Coins will not spawn."));
	}

	if (SmallObstacleClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Initializing Obstacle Pool with class: %s"), *SmallObstacleClass->GetName());
		ObstaclePool = MakeShared<FObjectPool<AObstacle>>();
		ObstaclePool->Initialize(GetWorld(), SmallObstacleClass, 15); // Pre-allocate 15 obstacles
		UE_LOG(LogTemp, Warning, TEXT("Obstacle Pool initialized with 15 obstacles"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SmallObstacleClass is NULL! Obstacles will not spawn."));
	}

	// 3. Initialize Lane Graph (will be populated after first tile)
	LaneGraph = MakeShared<FLaneGraph>();
	UE_LOG(LogTemp, Warning, TEXT("Lane Graph initialized"));

	// 4. Initialize Score BST
	ScoreBST = MakeShared<FScoreBST>();
	UE_LOG(LogTemp, Warning, TEXT("Score BST initialized"));

	// 5. Initialize Pool ID tracking maps
	CoinPoolIDs.Empty();
	ObstaclePoolIDs.Empty();

	UE_LOG(LogTemp, Warning, TEXT("=== Data Structures Initialization Complete ==="));
}

void ACPP_EndlessRunnerGameModeBase::CreateInitialFloorTiles()
{
	// Spawn first tile and initialize lane graph
	if (const AFloorTile* Tile = AddFloorTile(false))
	{
		// FIXED: Use accessor functions instead of direct access to protected members
		if (Tile->GetLeftLane() && Tile->GetCenterLane() && Tile->GetRightLane())
		{
			LaneSwitchValues.Add(Tile->GetLeftLane()->GetComponentLocation().Y);
			LaneSwitchValues.Add(Tile->GetCenterLane()->GetComponentLocation().Y);
			LaneSwitchValues.Add(Tile->GetRightLane()->GetComponentLocation().Y);

			UE_LOG(LogTemp, Warning, TEXT("Lane positions: Left=%.0f, Center=%.0f, Right=%.0f"),
				Tile->GetLeftLane()->GetComponentLocation().Y,
				Tile->GetCenterLane()->GetComponentLocation().Y,
				Tile->GetRightLane()->GetComponentLocation().Y);
		}
		else
		{
			// Fallback values if lanes are not set
			LaneSwitchValues.Add(-200.0f);
			LaneSwitchValues.Add(0.0f);
			LaneSwitchValues.Add(200.0f);
			UE_LOG(LogTemp, Warning, TEXT("Using fallback lane positions"));
		}

		// Initialize Graph with lane positions
		LaneGraph->Initialize(LaneSwitchValues);
		UE_LOG(LogTemp, Warning, TEXT("Lane Graph initialized with %d lanes"), LaneSwitchValues.Num());
	}

	AddFloorTile(false);
	AddFloorTile(false);

	// Create initial tiles
	UE_LOG(LogTemp, Warning, TEXT("Creating %d initial floor tiles"), NumInitialFloorTiles);
	for (int i = 0; i < NumInitialFloorTiles; i++)
	{
		AddFloorTile(true);
	}
}

const AFloorTile* ACPP_EndlessRunnerGameModeBase::AddFloorTile(const bool bSpawnItems)
{
	if (UWorld* World = GetWorld())
	{
		if (!FloorTileClass)
		{
			UE_LOG(LogTemp, Error, TEXT("FloorTileClass is not set! Cannot spawn floor tile."));
			return nullptr;
		}

		UE_LOG(LogTemp, Warning, TEXT("Spawning floor tile at location: %s"),
			*NextSpawnPoint.GetLocation().ToString());

		AFloorTile* Tile = World->SpawnActor<AFloorTile>(FloorTileClass, NextSpawnPoint);
		if (Tile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Floor tile spawned: %s"), *Tile->GetName());

			// QUEUE OPERATION: Enqueue new tile (O(1))
			FloorTileQueue->Enqueue(Tile);

			if (bSpawnItems)
			{
				// Use object pools instead of spawning new objects
				SpawnItemsUsingPool(Tile);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Tile %s will not spawn items (bSpawnItems = false)"), *Tile->GetName());
			}
			NextSpawnPoint = Tile->GetAttachTransform();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn floor tile!"));
		}
		return Tile;
	}
	return nullptr;
}

void ACPP_EndlessRunnerGameModeBase::SpawnItemsUsingPool(AFloorTile* Tile)
{
	if (!Tile)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItemsUsingPool: Tile is null!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== SpawnItemsUsingPool called for tile: %s ==="), *Tile->GetName());

	// Check if classes are set
	if (!CoinClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CoinClass is not set!"));
	}

	if (!SmallObstacleClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SmallObstacleClass is not set!"));
	}

	// FIXED: Use accessor functions instead of direct access to protected members
	if (!Tile->GetLeftLane() || !Tile->GetCenterLane() || !Tile->GetRightLane())
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile lanes are not initialized properly!"));
		return;
	}

	// Get lane positions using accessor functions
	TArray<UArrowComponent*> Lanes = {
		Tile->GetLeftLane(),
		Tile->GetCenterLane(),
		Tile->GetRightLane()
	};

	int32 spawnedItems = 0;

	for (int32 LaneIdx = 0; LaneIdx < Lanes.Num(); LaneIdx++)
	{
		// Check if lane is blocked in graph
		if (LaneGraph->IsLaneBlocked(LaneIdx))
		{
			UE_LOG(LogTemp, Warning, TEXT("Lane %d is blocked, skipping"), LaneIdx);
			continue;
		}

		const float RandVal = FMath::FRandRange(0.0f, 1.0f);

		// Check if lane component is valid
		if (!Lanes[LaneIdx])
		{
			UE_LOG(LogTemp, Warning, TEXT("Lane %d component is null, skipping"), LaneIdx);
			continue;
		}

		const FTransform& SpawnLocation = Lanes[LaneIdx]->GetComponentTransform();

		UE_LOG(LogTemp, Warning, TEXT("Lane %d: Random value = %.2f"), LaneIdx, RandVal);

		// ===== TEMPORARY TEST: DIRECT SPAWN (bypass object pool) =====
		// Spawn obstacle (10-30% chance)
		if (RandVal >= 0.1f && RandVal < 0.3f)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Attempting to DIRECTLY spawn obstacle in lane %d"), LaneIdx);

			// DIRECT SPAWN INSTEAD OF POOL (Temporary test)
			AObstacle* Obstacle = GetWorld()->SpawnActor<AObstacle>(SmallObstacleClass, SpawnLocation);
			if (Obstacle)
			{
				UE_LOG(LogTemp, Warning, TEXT("  SUCCESS: Obstacle spawned directly: %s at location: %s"),
					*Obstacle->GetName(),
					*SpawnLocation.GetLocation().ToString());

				// Assign a pool ID and track it
				int32 PoolID = GetNextPoolID();
				Obstacle->SetPoolID(PoolID);
				ObstaclePoolIDs.Add(Obstacle, PoolID);

				Tile->AddPooledActor(Obstacle);
				spawnedItems++;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED: Could not spawn obstacle directly!"));
			}
		}
		// Spawn coin (50-100% chance)
		else if (RandVal >= 0.5f)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Attempting to DIRECTLY spawn coin in lane %d"), LaneIdx);

			// DIRECT SPAWN INSTEAD OF POOL (Temporary test)
			ACoin* Coin = GetWorld()->SpawnActor<ACoin>(CoinClass, SpawnLocation);
			if (Coin)
			{
				UE_LOG(LogTemp, Warning, TEXT("  SUCCESS: Coin spawned directly: %s at location: %s"),
					*Coin->GetName(),
					*SpawnLocation.GetLocation().ToString());

				// Assign a pool ID and track it
				int32 PoolID = GetNextPoolID();
				Coin->SetPoolID(PoolID);
				CoinPoolIDs.Add(Coin, PoolID);

				Tile->AddPooledActor(Coin);
				spawnedItems++;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED: Could not spawn coin directly!"));
			}
		}
		/* ===== ORIGINAL OBJECT POOL CODE (COMMENTED OUT FOR TESTING) =====
		// Spawn obstacle from pool (10-30% chance)
		if (RandVal >= 0.1f && RandVal < 0.3f)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Attempting to spawn obstacle from pool in lane %d"), LaneIdx);

			// OBJECT POOL: Acquire obstacle from pool (O(1) hash map operation)
			AObstacle* Obstacle = ObstaclePool->Acquire(SpawnLocation);
			if (Obstacle)
			{
				// Assign a pool ID and track it
				int32 PoolID = GetNextPoolID();
				Obstacle->SetPoolID(PoolID);
				ObstaclePoolIDs.Add(Obstacle, PoolID);

				UE_LOG(LogTemp, Warning, TEXT("  SUCCESS: Obstacle acquired from pool: %s (Pool ID: %d)"),
					*Obstacle->GetName(), PoolID);

				Tile->AddPooledActor(Obstacle);
				spawnedItems++;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED: Could not acquire obstacle from pool!"));
			}
		}
		// Spawn coin from pool (50-100% chance)
		else if (RandVal >= 0.5f)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Attempting to spawn coin from pool in lane %d"), LaneIdx);

			// OBJECT POOL: Acquire coin from pool (O(1) hash map operation)
			ACoin* Coin = CoinPool->Acquire(SpawnLocation);
			if (Coin)
			{
				// Assign a pool ID and track it
				int32 PoolID = GetNextPoolID();
				Coin->SetPoolID(PoolID);
				CoinPoolIDs.Add(Coin, PoolID);

				UE_LOG(LogTemp, Warning, TEXT("  SUCCESS: Coin acquired from pool: %s (Pool ID: %d)"),
					*Coin->GetName(), PoolID);

				Tile->AddPooledActor(Coin);
				spawnedItems++;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED: Could not acquire coin from pool!"));
			}
		}
		*/
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Nothing to spawn in lane %d (RandVal = %.2f)"), LaneIdx, RandVal);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Total spawned items: %d ==="), spawnedItems);
}

void ACPP_EndlessRunnerGameModeBase::AddCoin()
{
	TotalCoins++;
	OnCoinsCountChanged.Broadcast(TotalCoins);

	// BST OPERATION: Insert score into BST (O(log n))
	ScoreBST->Insert(TotalCoins, TEXT("Player"));

	UE_LOG(LogTemp, Warning, TEXT("Coin collected! Total coins: %d"), TotalCoins);
}

void ACPP_EndlessRunnerGameModeBase::PlayerDied()
{
	CurrentLivesCount--;
	OnLivesCountChanged.Broadcast(CurrentLivesCount);

	UE_LOG(LogTemp, Warning, TEXT("Player died! Lives remaining: %d"), CurrentLivesCount);

	if (CurrentLivesCount > 0)
	{
		// QUEUE OPERATION: Dequeue and destroy all tiles
		UE_LOG(LogTemp, Warning, TEXT("Resetting level..."));
		while (!FloorTileQueue->IsEmpty())
		{
			if (AFloorTile* Tile = FloorTileQueue->Dequeue())
			{
				// Return pooled objects before destroying tile
				ReturnPooledObjects(Tile);
				Tile->DestroyFloorTile();
			}
		}

		// Reset state
		NextSpawnPoint = FTransform();

		// Create initial floor tiles
		CreateInitialFloorTiles();

		// Broadcast level reset
		OnLevelReset.Broadcast();
	}
	else
	{
		GameOver();
	}
}

void ACPP_EndlessRunnerGameModeBase::ReturnPooledObjects(AFloorTile* Tile)
{
	if (!Tile) return;

	// FIXED: Use accessor function instead of direct access
	const TArray<AActor*>& PooledActors = Tile->GetPooledActors();

	UE_LOG(LogTemp, Warning, TEXT("Returning %d pooled objects from tile: %s"),
		PooledActors.Num(), *Tile->GetName());

	// Return all pooled objects to their respective pools
	for (AActor* Actor : PooledActors)
	{
		if (ACoin* Coin = Cast<ACoin>(Actor))
		{
			// Get the tracked Pool ID
			if (int32* PoolID = CoinPoolIDs.Find(Coin))
			{
				UE_LOG(LogTemp, Warning, TEXT("  Returning coin: %s (Pool ID: %d)"),
					*Coin->GetName(), *PoolID);

				// Try to return to pool (but we're using direct spawn for now)
				// CoinPool->Release(*PoolID); // Commented out for direct spawn test

				// Destroy the coin since we're not using object pool yet
				Coin->Destroy();

				CoinPoolIDs.Remove(Coin);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  Coin %s has no tracked Pool ID! Destroying directly."),
					*Coin->GetName());
				Coin->Destroy();
			}
		}
		else if (AObstacle* Obstacle = Cast<AObstacle>(Actor))
		{
			if (int32* PoolID = ObstaclePoolIDs.Find(Obstacle))
			{
				UE_LOG(LogTemp, Warning, TEXT("  Returning obstacle: %s (Pool ID: %d)"),
					*Obstacle->GetName(), *PoolID);

				// Try to return to pool (but we're using direct spawn for now)
				// ObstaclePool->Release(*PoolID); // Commented out for direct spawn test

				// Destroy the obstacle since we're not using object pool yet
				Obstacle->Destroy();

				ObstaclePoolIDs.Remove(Obstacle);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  Obstacle %s has no tracked Pool ID! Destroying directly."),
					*Obstacle->GetName());
				Obstacle->Destroy();
			}
		}
	}

	// FIXED: Use accessor function instead of direct access
	Tile->ClearPooledActors();
}

void ACPP_EndlessRunnerGameModeBase::RemoveTile(AFloorTile* Tile)
{
	// QUEUE OPERATION: Remove specific tile
	UE_LOG(LogTemp, Warning, TEXT("Removing tile from queue: %s"), *Tile->GetName());

	TArray<AFloorTile*> AllTiles = FloorTileQueue->ToArray();
	FloorTileQueue->Clear();

	for (AFloorTile* T : AllTiles)
	{
		if (T != Tile)
		{
			FloorTileQueue->Enqueue(T);
		}
		else
		{
			ReturnPooledObjects(T);
		}
	}
}

void ACPP_EndlessRunnerGameModeBase::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("=== GAME OVER ==="));

	// Display top scores using BST
	TArray<FScoreNode*> TopScores = ScoreBST->GetTopScores(10);

	UE_LOG(LogTemp, Warning, TEXT("=== TOP 10 SCORES ==="));
	for (int32 i = 0; i < TopScores.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d. %s: %d"), i + 1, *TopScores[i]->PlayerName, TopScores[i]->Score);
	}

	if (IsValid(GameOverWidgetClass))
	{
		if (UUserWidget* Widget = CreateWidget(GetWorld(), GameOverWidgetClass))
		{
			Widget->AddToViewport();
		}
	}
}

// ALGORITHM: QuickSort for sorting scores
void ACPP_EndlessRunnerGameModeBase::QuickSortScores(TArray<int32>& Scores, int32 Low, int32 High)
{
	if (Low < High)
	{
		int32 PivotIndex = Partition(Scores, Low, High);
		QuickSortScores(Scores, Low, PivotIndex - 1);
		QuickSortScores(Scores, PivotIndex + 1, High);
	}
}

int32 ACPP_EndlessRunnerGameModeBase::Partition(TArray<int32>& Scores, int32 Low, int32 High)
{
	int32 Pivot = Scores[High];
	int32 i = Low - 1;

	for (int32 j = Low; j < High; j++)
	{
		if (Scores[j] <= Pivot)
		{
			i++;
			Swap(Scores[i], Scores[j]);
		}
	}
	Swap(Scores[i + 1], Scores[High]);
	return i + 1;
}

// ALGORITHM: Binary Search for finding score threshold
int32 ACPP_EndlessRunnerGameModeBase::BinarySearchScore(const TArray<int32>& SortedScores, int32 Target)
{
	int32 Left = 0;
	int32 Right = SortedScores.Num() - 1;

	while (Left <= Right)
	{
		int32 Mid = Left + (Right - Left) / 2;

		if (SortedScores[Mid] == Target)
			return Mid;

		if (SortedScores[Mid] < Target)
			Left = Mid + 1;
		else
			Right = Mid - 1;
	}

	return -1; // Not found
}

// GRAPH ALGORITHM: Find optimal lane using BFS
int32 ACPP_EndlessRunnerGameModeBase::FindOptimalLane(int32 CurrentLane, int32 TargetLane)
{
	// Use BFS to find shortest path between lanes
	TArray<int32> Path = LaneGraph->BFS_FindPath(CurrentLane, TargetLane);

	if (Path.Num() > 1)
	{
		// Return next lane in path
		return Path[1];
	}

	return CurrentLane; // Stay in current lane
}

// Mark lane as blocked (for AI obstacle avoidance)
void ACPP_EndlessRunnerGameModeBase::SetLaneBlocked(int32 LaneID, bool bBlocked)
{
	LaneGraph->SetLaneBlocked(LaneID, bBlocked);
	UE_LOG(LogTemp, Warning, TEXT("Lane %d blocked: %s"), LaneID, bBlocked ? TEXT("true") : TEXT("false"));
}