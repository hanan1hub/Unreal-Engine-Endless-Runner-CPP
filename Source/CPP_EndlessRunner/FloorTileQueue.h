// FloorTileQueue.h - Custom Queue Data Structure
#pragma once

#include "CoreMinimal.h"

// Forward declaration
class AFloorTile;

/**
 * Queue Node for Floor Tile Management
 * Part of custom Queue data structure implementation
 */
struct FFloorTileNode
{
    AFloorTile* Tile;
    FFloorTileNode* Next;

    FFloorTileNode(AFloorTile* InTile) : Tile(InTile), Next(nullptr) {}
};

/**
 * Custom Queue Data Structure (FIFO)
 * Used for managing floor tiles in order of spawning
 * Time Complexity: O(1) for all operations
 */
class FFloorTileQueue
{
private:
    FFloorTileNode* Front;
    FFloorTileNode* Rear;
    int32 Size;

public:
    FFloorTileQueue();
    ~FFloorTileQueue();

    // Queue Operations - O(1) complexity
    void Enqueue(AFloorTile* Tile);      // Add to rear
    AFloorTile* Dequeue();                // Remove from front
    AFloorTile* Peek() const;             // View front without removing
    
    // Utility functions
    bool IsEmpty() const { return Size == 0; }
    int32 GetSize() const { return Size; }
    void Clear();
    
    // Get all tiles as array for iteration
    TArray<AFloorTile*> ToArray() const;
};

// ===== IMPLEMENTATION =====

inline FFloorTileQueue::FFloorTileQueue() 
    : Front(nullptr), Rear(nullptr), Size(0) 
{
}

inline FFloorTileQueue::~FFloorTileQueue()
{
    Clear();
}

inline void FFloorTileQueue::Enqueue(AFloorTile* Tile)
{
    FFloorTileNode* NewNode = new FFloorTileNode(Tile);
    
    if (IsEmpty())
    {
        Front = Rear = NewNode;
    }
    else
    {
        Rear->Next = NewNode;
        Rear = NewNode;
    }
    Size++;
}

inline AFloorTile* FFloorTileQueue::Dequeue()
{
    if (IsEmpty())
    {
        return nullptr;
    }

    FFloorTileNode* Temp = Front;
    AFloorTile* Tile = Front->Tile;
    
    Front = Front->Next;
    
    if (Front == nullptr)
    {
        Rear = nullptr;
    }
    
    delete Temp;
    Size--;
    
    return Tile;
}

inline AFloorTile* FFloorTileQueue::Peek() const
{
    return IsEmpty() ? nullptr : Front->Tile;
}

inline void FFloorTileQueue::Clear()
{
    while (!IsEmpty())
    {
        Dequeue();
    }
}

inline TArray<AFloorTile*> FFloorTileQueue::ToArray() const
{
    TArray<AFloorTile*> Result;
    FFloorTileNode* Current = Front;
    
    while (Current != nullptr)
    {
        Result.Add(Current->Tile);
        Current = Current->Next;
    }
    
    return Result;
}