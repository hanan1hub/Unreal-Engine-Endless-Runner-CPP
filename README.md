# Endless Runner Game - Unreal Engine 5.7

A high-performance 3D endless runner game built with Unreal Engine 5.7, demonstrating advanced data structures and algorithms in real-time game development.

![Game Performance](https://img.shields.io/badge/FPS-111--142-brightgreen)
![Memory Optimization](https://img.shields.io/badge/Memory%20Saved-38%25-blue)
![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-purple)

## 🎮 Overview

This fully functional 3D endless runner features three progressive difficulty levels, achieving consistent 60+ FPS performance through optimized data structures and algorithms. The game showcases practical implementation of computer science fundamentals in a real-time gaming environment.

## ✨ Key Features

- **Three Difficulty Levels**: Easy, Medium, and Hard with progressive complexity
- **Optimized Performance**: 111-142 FPS with 38% memory reduction
- **Advanced Data Structures**: Queue, Hash Map, Graph, Binary Search Tree
- **Sophisticated Algorithms**: BFS, DFS, QuickSort, Binary Search
- **Object Pooling System**: Zero garbage collection pauses
- **Procedural Generation**: Dynamic item spawning based on difficulty

## 🛠️ Technical Achievements

### Data Structures Implemented

1. **Queue** - O(1) tile management for seamless endless progression
2. **Hash Map** - Object pooling system reducing memory usage by 38%
3. **Graph** - Adjacency list representation for 3-lane navigation
4. **Binary Search Tree** - O(log n) score management for leaderboards
5. **Dynamic Arrays** - Flexible collection storage
6. **Implicit Stack** - Recursion-based tree/graph traversals

### Algorithms Implemented

1. **Breadth-First Search** - Shortest pathfinding between lanes O(V+E)
2. **Depth-First Search** - Alternative path exploration
3. **QuickSort** - Score ordering for leaderboard display O(n log n)
4. **Binary Search** - Efficient score lookup O(log n)
5. **Procedural Generation** - Randomized item spawning per level
6. **Tree Traversals** - In-order, pre-order, post-order for BST operations

## 🚀 Performance Metrics

| Metric | Before Optimization | After Optimization | Improvement |
|--------|-------------------|-------------------|-------------|
| Frame Time | 18-22ms | 7-9ms | 60% faster |
| FPS | 45-55 | 111-142 | 150% increase |
| Memory Usage | 450MB | 280MB | 38% reduction |
| GC Pauses | 15-20/min | 0 | 100% eliminated |

## 📋 System Requirements

- **Operating System**: Windows 10/11 (64-bit)
- **Processor**: Intel Core i5 or equivalent
- **Memory**: 4 GB RAM minimum
- **Graphics**: DirectX 11 compatible GPU
- **Storage**: 500 MB available space

## 🎯 Installation & Setup

### Option 1: Download Executable

1. Download the game executable from the [Drive Link](https://drive.google.com/file/d/1BzoJOW8FSr73U5mjwbS_sqKXhf0zcrnQ/view?usp=drive_link)
2. Extract the downloaded archive
3. Run `EndlessRunner.exe`

### Option 2: Build from Source

1. Clone the repository:
```bash
git clone https://github.com/hanan1hub/Unreal-Engine-Endless-Runner-CPP.git
```

2. Open the project in Unreal Engine 5.7

3. Build the project:
   - For Development: `Development Editor`
   - For Shipping: `Shipping`

4. Launch the game from the editor or build the executable

## 🎮 How to Play

### Controls

- **A/Left Arrow** - Move to left lane
- **D/Right Arrow** - Move to right lane
- **Space** - Jump
- **Ctrl** - Slide
- **ESC** - Pause game

### Gameplay

1. Start the game from the main menu
2. Collect coins to increase your score
3. Avoid obstacles by switching lanes, jumping, or sliding
4. Survive as long as possible to achieve a high score
5. Compete on the leaderboard

### Difficulty Levels

- **Easy**: 50% coin spawn rate, slower speed
- **Medium**: 40% coin spawn rate, moderate speed
- **Hard**: 30% coin spawn rate, maximum speed

## 🏗️ System Architecture

```
AActor (Base)
├── AFloorTile (Floor segments)
├── ACoin (Collectible items)
└── AObstacle (Hazards)

ACharacter (Base)
└── ARunCharacter (Player-controlled runner)

UUserWidget (Base UI)
├── UGameHUDWidget (In-game display)
├── UMainMenuWidget (Entry point)
├── UPauseMenuWidget (Pause screen)
└── UGameOverWidget (End screen)
```

## 📊 Data Structure Performance

| Operation | Scale | Time | Complexity |
|-----------|-------|------|------------|
| Queue Enqueue | 10,000 ops | 0.82ms | O(1) |
| Hash Map Lookup | 1,000,000 ops | 45.3ms | O(1) |
| BFS Pathfinding | 1,000 paths | 0.31ms | O(V+E) |
| BST Insert | 10,000 scores | 8.7ms | O(log n) |

## 🔧 Optimization Strategies

1. **Object Pooling**: Pre-allocated 35 objects with hash map tracking
2. **Early Exit BFS**: 40% faster pathfinding with immediate return
3. **Array Pre-allocation**: Prevents runtime reallocations
4. **Load Factor Management**: Maintains 0.75 hash map load factor

## 👤 Author

**Hanan Majeed** (CMS ID: 519166)

- Project Lead & System Architect
- GameMode design and data structure integration
- UI and graphics implementation
- Performance optimization and testing

## 🤝 Contributing

This project was developed as part of a Data Structures and Algorithms course project. While the main development is complete, suggestions and feedback are welcome.

## 📝 License

This project is part of an academic assignment. Please respect academic integrity policies when using or referencing this code.

## 🔗 Links

- **GitHub Repository**: [Unreal Engine Endless Runner](https://github.com/hanan1hub/Unreal-Engine-Endless-Runner-CPP.git)
- **Demo Video**: [Watch on Drive](https://drive.google.com/file/d/1FQUVej5CsCUt2UE2hx9aymFjGX0eeott/view?usp=drive_link)
- **Executable Download**: [Download from Drive](https://drive.google.com/file/d/1BzoJOW8FSr73U5mjwbS_sqKXhf0zcrnQ/view?usp=drive_link)

## 📚 Documentation

For detailed technical documentation, including complexity analysis, design decisions, and implementation details, please refer to the full project report.

### Key Sections

- System Architecture and Class Hierarchy
- Data Structure Justifications with Complexity Analysis
- Algorithm Analysis with Big-O Notation
- Performance Benchmarks and Optimization Strategies
- Code Snippets with Explanations

## 🎓 Learning Outcomes

This project demonstrates:

- Practical application of data structures in game development
- Real-time performance optimization techniques
- Object-oriented design patterns in Unreal Engine
- Memory management and garbage collection strategies
- Algorithm selection based on performance requirements

## 🐛 Known Issues

- None currently reported

## 📞 Contact

For questions or feedback regarding this project:
- GitHub: [hanan1hub](https://github.com/hanan1hub)

---

**Note**: This project was developed as an educational exercise demonstrating the practical application of data structures and algorithms in game development using Unreal Engine 5.7.
