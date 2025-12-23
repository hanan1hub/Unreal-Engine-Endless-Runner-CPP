// LaneGraph.h - Graph Data Structure for Lane Management
#pragma once

#include "CoreMinimal.h"

/**
 * Graph Node representing a Lane
 */
struct FLaneNode
{
    int32 LaneID;               // 0 = Left, 1 = Center, 2 = Right
    float YPosition;            // Y-axis position in world
    TArray<int32> Neighbors;    // Adjacent lanes (graph edges)
    bool bIsBlocked;            // For pathfinding
    
    FLaneNode() : LaneID(-1), YPosition(0.0f), bIsBlocked(false) {}
    FLaneNode(int32 ID, float YPos) : LaneID(ID), YPosition(YPos), bIsBlocked(false) {}
};

/**
 * Graph Data Structure for Lane System
 * Uses Adjacency List representation
 * Implements BFS and DFS for pathfinding
 * Time Complexity: O(V + E) for traversal algorithms
 */
class FLaneGraph
{
private:
    // Adjacency List: Graph representation
    TMap<int32, FLaneNode> Nodes;
    int32 NumLanes;

public:
    FLaneGraph();
    
    // Graph Construction
    void Initialize(const TArray<float>& LanePositions);
    void AddEdge(int32 FromLane, int32 ToLane);
    void RemoveEdge(int32 FromLane, int32 ToLane);
    
    // Node operations
    void SetLaneBlocked(int32 LaneID, bool bBlocked);
    bool IsLaneBlocked(int32 LaneID) const;
    float GetLanePosition(int32 LaneID) const;
    
    // Graph Traversal Algorithms
    TArray<int32> BFS_FindPath(int32 StartLane, int32 TargetLane) const;
    TArray<int32> DFS_FindPath(int32 StartLane, int32 TargetLane) const;
    
    // Shortest path (for AI or optimal lane selection)
    TArray<int32> FindShortestPath(int32 StartLane, int32 TargetLane) const;
    
    // Utility
    int32 GetNumLanes() const { return NumLanes; }
    bool IsValidLane(int32 LaneID) const;
    TArray<int32> GetNeighbors(int32 LaneID) const;
    
private:
    // Helper functions for traversal
    bool DFS_Recursive(int32 Current, int32 Target, TSet<int32>& Visited, TArray<int32>& Path) const;
};

// ===== IMPLEMENTATION =====

inline FLaneGraph::FLaneGraph() : NumLanes(0) {}

inline void FLaneGraph::Initialize(const TArray<float>& LanePositions)
{
    NumLanes = LanePositions.Num();
    
    // Create nodes (vertices)
    for (int32 i = 0; i < NumLanes; i++)
    {
        FLaneNode Node(i, LanePositions[i]);
        Nodes.Add(i, Node);
    }
    
    // Create edges (adjacent lanes can connect)
    // Left <-> Center <-> Right
    if (NumLanes >= 2)
    {
        AddEdge(0, 1); // Left to Center
        AddEdge(1, 0); // Center to Left
    }
    if (NumLanes >= 3)
    {
        AddEdge(1, 2); // Center to Right
        AddEdge(2, 1); // Right to Center
    }
}

inline void FLaneGraph::AddEdge(int32 FromLane, int32 ToLane)
{
    if (FLaneNode* Node = Nodes.Find(FromLane))
    {
        if (!Node->Neighbors.Contains(ToLane))
        {
            Node->Neighbors.Add(ToLane);
        }
    }
}

inline void FLaneGraph::RemoveEdge(int32 FromLane, int32 ToLane)
{
    if (FLaneNode* Node = Nodes.Find(FromLane))
    {
        Node->Neighbors.Remove(ToLane);
    }
}

inline void FLaneGraph::SetLaneBlocked(int32 LaneID, bool bBlocked)
{
    if (FLaneNode* Node = Nodes.Find(LaneID))
    {
        Node->bIsBlocked = bBlocked;
    }
}

inline bool FLaneGraph::IsLaneBlocked(int32 LaneID) const
{
    if (const FLaneNode* Node = Nodes.Find(LaneID))
    {
        return Node->bIsBlocked;
    }
    return true;
}

inline float FLaneGraph::GetLanePosition(int32 LaneID) const
{
    if (const FLaneNode* Node = Nodes.Find(LaneID))
    {
        return Node->YPosition;
    }
    return 0.0f;
}

inline bool FLaneGraph::IsValidLane(int32 LaneID) const
{
    return Nodes.Contains(LaneID);
}

inline TArray<int32> FLaneGraph::GetNeighbors(int32 LaneID) const
{
    if (const FLaneNode* Node = Nodes.Find(LaneID))
    {
        return Node->Neighbors;
    }
    return TArray<int32>();
}

// BFS Algorithm - Finds shortest path in unweighted graph
inline TArray<int32> FLaneGraph::BFS_FindPath(int32 StartLane, int32 TargetLane) const
{
    TArray<int32> Path;
    
    if (!IsValidLane(StartLane) || !IsValidLane(TargetLane))
        return Path;
    
    // Queue for BFS
    TQueue<int32> Queue;
    TSet<int32> Visited;
    TMap<int32, int32> Parent; // For path reconstruction
    
    Queue.Enqueue(StartLane);
    Visited.Add(StartLane);
    Parent.Add(StartLane, -1);
    
    bool bFound = false;
    
    while (!Queue.IsEmpty() && !bFound)
    {
        int32 Current;
        Queue.Dequeue(Current);
        
        if (Current == TargetLane)
        {
            bFound = true;
            break;
        }
        
        const FLaneNode* CurrentNode = Nodes.Find(Current);
        if (!CurrentNode || CurrentNode->bIsBlocked)
            continue;
        
        // Explore neighbors
        for (int32 Neighbor : CurrentNode->Neighbors)
        {
            if (!Visited.Contains(Neighbor) && !IsLaneBlocked(Neighbor))
            {
                Queue.Enqueue(Neighbor);
                Visited.Add(Neighbor);
                Parent.Add(Neighbor, Current);
            }
        }
    }
    
    // Reconstruct path
    if (bFound)
    {
        int32 Current = TargetLane;
        while (Current != -1)
        {
            Path.Insert(Current, 0);
            Current = Parent[Current];
        }
    }
    
    return Path;
}

// DFS Algorithm - Depth-first search
inline TArray<int32> FLaneGraph::DFS_FindPath(int32 StartLane, int32 TargetLane) const
{
    TArray<int32> Path;
    TSet<int32> Visited;
    
    if (DFS_Recursive(StartLane, TargetLane, Visited, Path))
    {
        return Path;
    }
    
    Path.Empty();
    return Path;
}

inline bool FLaneGraph::DFS_Recursive(int32 Current, int32 Target, TSet<int32>& Visited, TArray<int32>& Path) const
{
    if (!IsValidLane(Current) || IsLaneBlocked(Current))
        return false;
    
    Visited.Add(Current);
    Path.Add(Current);
    
    if (Current == Target)
        return true;
    
    const FLaneNode* CurrentNode = Nodes.Find(Current);
    if (CurrentNode)
    {
        for (int32 Neighbor : CurrentNode->Neighbors)
        {
            if (!Visited.Contains(Neighbor))
            {
                if (DFS_Recursive(Neighbor, Target, Visited, Path))
                    return true;
            }
        }
    }
    
    Path.RemoveAt(Path.Num() - 1);
    return false;
}

inline TArray<int32> FLaneGraph::FindShortestPath(int32 StartLane, int32 TargetLane) const
{
    // BFS always finds shortest path in unweighted graph
    return BFS_FindPath(StartLane, TargetLane);
}