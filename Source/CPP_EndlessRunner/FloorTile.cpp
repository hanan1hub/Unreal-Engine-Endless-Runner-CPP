// FloorTile.cpp - Modified for Object Pool System
#include "FloorTile.h"
#include "CPP_EndlessRunnerGameModeBase.h"
#include "Obstacle.h"
#include "RunCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Coin.h"

AFloorTile::AFloorTile()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(SceneComponent);

	AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	AttachPoint->SetupAttachment(SceneComponent);

	CenterLane = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterLane"));
	CenterLane->SetupAttachment(SceneComponent);

	RightLane = CreateDefaultSubobject<UArrowComponent>(TEXT("RightLane"));
	RightLane->SetupAttachment(SceneComponent);

	LeftLane = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftLane"));
	LeftLane->SetupAttachment(SceneComponent);

	FloorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorTriggerBox"));
	FloorTriggerBox->SetupAttachment(SceneComponent);
	FloorTriggerBox->SetBoxExtent(FVector(32.0f, 500.0f, 200.0f));
	FloorTriggerBox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	DestroyAfterTime = 1.25f;
}

void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	// Get and cache reference to Game Mode
	GameMode = Cast<ACPP_EndlessRunnerGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	check(GameMode);

	// Bind overlap event
	FloorTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::OnTriggerOverlap);
}

void AFloorTile::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if player overlapped with this tile
	if (ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor))
	{
		// Tell GameMode to spawn next tile (using Queue)
		GameMode->AddFloorTile(true);

		// Set timer to destroy this tile after delay
		GetWorldTimerManager().SetTimer(DestroyHandle, this, &AFloorTile::DestroyFloorTile, DestroyAfterTime, false);
	}
}

void AFloorTile::DestroyFloorTile()
{
	// Clear the timer if it's still valid
	if (DestroyHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DestroyHandle);
	}

	// ===== IMPORTANT CHANGE FOR OBJECT POOLING =====
	// OLD CODE (REMOVED):
	// We used to destroy all child actors here
	// for(auto Child : ChildActors)
	// {
	//     if(IsValid(Child))
	//     {
	//         Child->Destroy();
	//     }
	// }
	// ChildActors.Empty();

	// NEW CODE:
	// PooledActors are NOT destroyed here!
	// They are automatically returned to their pools by GameMode
	// before this function is called via GameMode->RemoveTile()
	// We just clear the reference array
	PooledActors.Empty();

	// Remove this tile from GameMode's Queue
	GameMode->RemoveTile(this);

	// Finally, destroy this floor tile actor
	this->Destroy();
}

void AFloorTile::SpawnItems()
{
	// ===== DEPRECATED =====
	// This function is now handled by GameMode->SpawnItemsUsingPool()
	// which uses object pooling for better performance

	// Keeping this function empty for backward compatibility
	// If you have Blueprint references to this, they won't break

	// The GameMode now handles all spawning through:
	// - CoinPool (Hash Map based object pool)
	// - ObstaclePool (Hash Map based object pool)
}

void AFloorTile::SpawnLaneItem(const UArrowComponent* Lane, int32& BigObstaclesCount)
{
	// ===== DEPRECATED =====
	// This function is now handled by GameMode's object pooling system

	// Keeping this function empty for backward compatibility
	// If you have Blueprint references to this, they won't break

	// The new system uses:
	// - Object pools for efficient reuse
	// - Hash maps for O(1) lookup
	// - Graph-based lane management
}