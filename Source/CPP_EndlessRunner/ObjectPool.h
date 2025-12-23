// ObjectPool.h - Hash Map based Object Pooling System
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

/**
 * Object Pool using Hash Map for O(1) lookup
 * Efficiently manages and reuses game objects (coins, obstacles)
 * Reduces memory allocation and garbage collection overhead
 */
template<typename T>
class FObjectPool
{
private:
    // Hash Map: Key = ObjectID, Value = Object pointer
    TMap<int32, T*> ActiveObjects;
    
    // Stack (using TArray) for available objects (LIFO)
    TArray<T*> AvailableObjects;
    
    // Reference to spawning class
    TSubclassOf<T> ObjectClass;
    UWorld* World;
    
    int32 NextObjectID;
    int32 PoolSize;

public:
    FObjectPool();
    
    void Initialize(UWorld* InWorld, TSubclassOf<T> InClass, int32 InitialSize = 10);
    
    // Pool Operations
    T* Acquire(const FTransform& SpawnTransform);  // O(1) - Get from pool
    void Release(int32 ObjectID);                   // O(1) - Return to pool
    void ReleaseAll();                              // Clear all active
    
    // Hash Map operations
    T* FindActive(int32 ObjectID) const;            // O(1) lookup
    bool IsActive(int32 ObjectID) const;            // O(1) check
    
    // Getters
    int32 GetActiveCount() const { return ActiveObjects.Num(); }
    int32 GetAvailableCount() const { return AvailableObjects.Num(); }
    int32 GetTotalSize() const { return PoolSize; }
    
    // Cleanup
    void Destroy();

private:
    T* CreateNewObject(const FTransform& SpawnTransform);
    void ExpandPool(int32 AdditionalSize);
};

// ===== TEMPLATE IMPLEMENTATION =====

template<typename T>
FObjectPool<T>::FObjectPool() 
    : World(nullptr), NextObjectID(0), PoolSize(0)
{
}

template<typename T>
void FObjectPool<T>::Initialize(UWorld* InWorld, TSubclassOf<T> InClass, int32 InitialSize)
{
    World = InWorld;
    ObjectClass = InClass;
    PoolSize = InitialSize;
    
    // Pre-allocate objects
    for (int32 i = 0; i < InitialSize; i++)
    {
        FTransform SpawnTransform;
        T* NewObject = CreateNewObject(SpawnTransform);
        
        if (NewObject)
        {
            // Deactivate and add to available pool
            AActor* Actor = Cast<AActor>(NewObject);
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                Actor->SetActorEnableCollision(false);
            }
            AvailableObjects.Push(NewObject);
        }
    }
}

template<typename T>
T* FObjectPool<T>::Acquire(const FTransform& SpawnTransform)
{
    T* Object = nullptr;
    
    // Check if available objects exist (Stack Pop - O(1))
    if (AvailableObjects.Num() > 0)
    {
        Object = AvailableObjects.Pop();
    }
    else
    {
        // Pool exhausted, create new object
        ExpandPool(5); // Grow by 5
        if (AvailableObjects.Num() > 0)
        {
            Object = AvailableObjects.Pop();
        }
    }
    
    if (Object)
    {
        // Activate object
        AActor* Actor = Cast<AActor>(Object);
        if (Actor)
        {
            Actor->SetActorTransform(SpawnTransform);
            Actor->SetActorHiddenInGame(false);
            Actor->SetActorEnableCollision(true);
        }
        
        // Add to active hash map with unique ID (O(1) insertion)
        int32 ObjectID = NextObjectID++;
        ActiveObjects.Add(ObjectID, Object);
        
        return Object;
    }
    
    return nullptr;
}

template<typename T>
void FObjectPool<T>::Release(int32 ObjectID)
{
    // O(1) lookup in hash map
    if (T** FoundObject = ActiveObjects.Find(ObjectID))
    {
        T* Object = *FoundObject;
        
        // Deactivate
        AActor* Actor = Cast<AActor>(Object);
        if (Actor)
        {
            Actor->SetActorHiddenInGame(true);
            Actor->SetActorEnableCollision(false);
        }
        
        // Remove from active (O(1))
        ActiveObjects.Remove(ObjectID);
        
        // Return to available pool (Stack Push - O(1))
        AvailableObjects.Push(Object);
    }
}

template<typename T>
void FObjectPool<T>::ReleaseAll()
{
    // Move all active objects back to available
    for (auto& Pair : ActiveObjects)
    {
        T* Object = Pair.Value;
        AActor* Actor = Cast<AActor>(Object);
        
        if (Actor)
        {
            Actor->SetActorHiddenInGame(true);
            Actor->SetActorEnableCollision(false);
        }
        
        AvailableObjects.Push(Object);
    }
    
    ActiveObjects.Empty();
}

template<typename T>
T* FObjectPool<T>::FindActive(int32 ObjectID) const
{
    // O(1) hash map lookup
    if (const T* const* FoundObject = ActiveObjects.Find(ObjectID))
    {
        return *FoundObject;
    }
    return nullptr;
}

template<typename T>
bool FObjectPool<T>::IsActive(int32 ObjectID) const
{
    return ActiveObjects.Contains(ObjectID); // O(1)
}

template<typename T>
T* FObjectPool<T>::CreateNewObject(const FTransform& SpawnTransform)
{
    if (World && ObjectClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        return World->SpawnActor<T>(ObjectClass, SpawnTransform, SpawnParams);
    }
    return nullptr;
}

template<typename T>
void FObjectPool<T>::ExpandPool(int32 AdditionalSize)
{
    for (int32 i = 0; i < AdditionalSize; i++)
    {
        FTransform SpawnTransform;
        T* NewObject = CreateNewObject(SpawnTransform);
        
        if (NewObject)
        {
            AActor* Actor = Cast<AActor>(NewObject);
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                Actor->SetActorEnableCollision(false);
            }
            AvailableObjects.Push(NewObject);
            PoolSize++;
        }
    }
}

template<typename T>
void FObjectPool<T>::Destroy()
{
    // Destroy all objects
    for (T* Object : AvailableObjects)
    {
        if (AActor* Actor = Cast<AActor>(Object))
        {
            Actor->Destroy();
        }
    }
    
    for (auto& Pair : ActiveObjects)
    {
        if (AActor* Actor = Cast<AActor>(Pair.Value))
        {
            Actor->Destroy();
        }
    }
    
    AvailableObjects.Empty();
    ActiveObjects.Empty();
}