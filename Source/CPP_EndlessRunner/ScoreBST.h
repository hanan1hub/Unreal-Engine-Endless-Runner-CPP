// ScoreBST.h - Binary Search Tree for Score Management
#pragma once

#include "CoreMinimal.h"

/**
 * BST Node for storing player scores
 */
struct FScoreNode
{
    int32 Score;
    FString PlayerName;
    FScoreNode* Left;
    FScoreNode* Right;
    int32 Height; // For AVL balancing (optional)
    
    FScoreNode(int32 InScore, const FString& InName)
        : Score(InScore), PlayerName(InName), Left(nullptr), Right(nullptr), Height(1)
    {}
};

/**
 * Binary Search Tree for managing scores
 * Supports: Insert, Search, Delete, Traversal
 * Time Complexity: O(log n) average for all operations
 */
class FScoreBST
{
private:
    FScoreNode* Root;
    int32 NodeCount;

public:
    FScoreBST();
    ~FScoreBST();
    
    // BST Operations
    void Insert(int32 Score, const FString& PlayerName);
    bool Search(int32 Score) const;
    void Delete(int32 Score);
    
    // Traversal Methods (different orderings)
    TArray<FScoreNode*> InOrderTraversal() const;     // Sorted ascending
    TArray<FScoreNode*> PreOrderTraversal() const;
    TArray<FScoreNode*> PostOrderTraversal() const;
    
    // Utility functions
    int32 FindMin() const;
    int32 FindMax() const;
    int32 GetHeight() const;
    int32 GetNodeCount() const { return NodeCount; }
    bool IsEmpty() const { return Root == nullptr; }
    
    // Get top N scores (for leaderboard)
    TArray<FScoreNode*> GetTopScores(int32 Count) const;
    
    // Clear tree
    void Clear();

private:
    // Helper recursive functions
    FScoreNode* InsertRecursive(FScoreNode* Node, int32 Score, const FString& PlayerName);
    bool SearchRecursive(FScoreNode* Node, int32 Score) const;
    FScoreNode* DeleteRecursive(FScoreNode* Node, int32 Score);
    
    FScoreNode* FindMinNode(FScoreNode* Node) const;
    FScoreNode* FindMaxNode(FScoreNode* Node) const;
    
    void InOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const;
    void PreOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const;
    void PostOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const;
    
    int32 GetHeightRecursive(FScoreNode* Node) const;
    void ClearRecursive(FScoreNode* Node);
};

// ===== IMPLEMENTATION =====

inline FScoreBST::FScoreBST() : Root(nullptr), NodeCount(0) {}

inline FScoreBST::~FScoreBST()
{
    Clear();
}

inline void FScoreBST::Insert(int32 Score, const FString& PlayerName)
{
    Root = InsertRecursive(Root, Score, PlayerName);
    NodeCount++;
}

inline FScoreNode* FScoreBST::InsertRecursive(FScoreNode* Node, int32 Score, const FString& PlayerName)
{
    // Base case: empty spot found
    if (Node == nullptr)
    {
        return new FScoreNode(Score, PlayerName);
    }
    
    // Recursive insertion
    if (Score < Node->Score)
    {
        Node->Left = InsertRecursive(Node->Left, Score, PlayerName);
    }
    else if (Score > Node->Score)
    {
        Node->Right = InsertRecursive(Node->Right, Score, PlayerName);
    }
    // If equal, allow duplicates by going right
    else
    {
        Node->Right = InsertRecursive(Node->Right, Score, PlayerName);
    }
    
    return Node;
}

inline bool FScoreBST::Search(int32 Score) const
{
    return SearchRecursive(Root, Score);
}

inline bool FScoreBST::SearchRecursive(FScoreNode* Node, int32 Score) const
{
    if (Node == nullptr)
        return false;
    
    if (Score == Node->Score)
        return true;
    
    if (Score < Node->Score)
        return SearchRecursive(Node->Left, Score);
    else
        return SearchRecursive(Node->Right, Score);
}

inline void FScoreBST::Delete(int32 Score)
{
    Root = DeleteRecursive(Root, Score);
    NodeCount--;
}

inline FScoreNode* FScoreBST::DeleteRecursive(FScoreNode* Node, int32 Score)
{
    if (Node == nullptr)
        return nullptr;
    
    // Find node to delete
    if (Score < Node->Score)
    {
        Node->Left = DeleteRecursive(Node->Left, Score);
    }
    else if (Score > Node->Score)
    {
        Node->Right = DeleteRecursive(Node->Right, Score);
    }
    else
    {
        // Node found - handle 3 cases
        
        // Case 1: No children (leaf node)
        if (Node->Left == nullptr && Node->Right == nullptr)
        {
            delete Node;
            return nullptr;
        }
        
        // Case 2: One child
        if (Node->Left == nullptr)
        {
            FScoreNode* Temp = Node->Right;
            delete Node;
            return Temp;
        }
        if (Node->Right == nullptr)
        {
            FScoreNode* Temp = Node->Left;
            delete Node;
            return Temp;
        }
        
        // Case 3: Two children
        // Find inorder successor (min in right subtree)
        FScoreNode* Successor = FindMinNode(Node->Right);
        Node->Score = Successor->Score;
        Node->PlayerName = Successor->PlayerName;
        Node->Right = DeleteRecursive(Node->Right, Successor->Score);
    }
    
    return Node;
}

inline FScoreNode* FScoreBST::FindMinNode(FScoreNode* Node) const
{
    while (Node && Node->Left != nullptr)
    {
        Node = Node->Left;
    }
    return Node;
}

inline FScoreNode* FScoreBST::FindMaxNode(FScoreNode* Node) const
{
    while (Node && Node->Right != nullptr)
    {
        Node = Node->Right;
    }
    return Node;
}

inline int32 FScoreBST::FindMin() const
{
    FScoreNode* MinNode = FindMinNode(Root);
    return MinNode ? MinNode->Score : 0;
}

inline int32 FScoreBST::FindMax() const
{
    FScoreNode* MaxNode = FindMaxNode(Root);
    return MaxNode ? MaxNode->Score : 0;
}

// In-Order Traversal: Left -> Root -> Right (gives sorted order)
inline TArray<FScoreNode*> FScoreBST::InOrderTraversal() const
{
    TArray<FScoreNode*> Result;
    InOrderRecursive(Root, Result);
    return Result;
}

inline void FScoreBST::InOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const
{
    if (Node == nullptr)
        return;
    
    InOrderRecursive(Node->Left, Result);
    Result.Add(Node);
    InOrderRecursive(Node->Right, Result);
}

// Pre-Order Traversal: Root -> Left -> Right
inline TArray<FScoreNode*> FScoreBST::PreOrderTraversal() const
{
    TArray<FScoreNode*> Result;
    PreOrderRecursive(Root, Result);
    return Result;
}

inline void FScoreBST::PreOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const
{
    if (Node == nullptr)
        return;
    
    Result.Add(Node);
    PreOrderRecursive(Node->Left, Result);
    PreOrderRecursive(Node->Right, Result);
}

// Post-Order Traversal: Left -> Right -> Root
inline TArray<FScoreNode*> FScoreBST::PostOrderTraversal() const
{
    TArray<FScoreNode*> Result;
    PostOrderRecursive(Root, Result);
    return Result;
}

inline void FScoreBST::PostOrderRecursive(FScoreNode* Node, TArray<FScoreNode*>& Result) const
{
    if (Node == nullptr)
        return;
    
    PostOrderRecursive(Node->Left, Result);
    PostOrderRecursive(Node->Right, Result);
    Result.Add(Node);
}

inline int32 FScoreBST::GetHeight() const
{
    return GetHeightRecursive(Root);
}

inline int32 FScoreBST::GetHeightRecursive(FScoreNode* Node) const
{
    if (Node == nullptr)
        return 0;
    
    int32 LeftHeight = GetHeightRecursive(Node->Left);
    int32 RightHeight = GetHeightRecursive(Node->Right);
    
    return 1 + FMath::Max(LeftHeight, RightHeight);
}

// Get top N highest scores (reverse in-order traversal)
inline TArray<FScoreNode*> FScoreBST::GetTopScores(int32 Count) const
{
    TArray<FScoreNode*> AllScores = InOrderTraversal();
    TArray<FScoreNode*> TopScores;
    
    // Reverse to get highest first
    for (int32 i = AllScores.Num() - 1; i >= 0 && TopScores.Num() < Count; i--)
    {
        TopScores.Add(AllScores[i]);
    }
    
    return TopScores;
}

inline void FScoreBST::Clear()
{
    ClearRecursive(Root);
    Root = nullptr;
    NodeCount = 0;
}

inline void FScoreBST::ClearRecursive(FScoreNode* Node)
{
    if (Node == nullptr)
        return;
    
    ClearRecursive(Node->Left);
    ClearRecursive(Node->Right);
    delete Node;
}