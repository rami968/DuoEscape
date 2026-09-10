# DuoEscape - Technical Architecture & Engine Design

This document details the software architecture, design patterns, and core systems powering **DuoEscape**, a C++20 2D cooperative puzzle game engine.

---

## 🏗️ System Overview & Component Diagram

```
                              ┌────────────────────────┐
                              │       main.cpp         │
                              └───────────┬────────────┘
                                          │
                                          ▼
                              ┌────────────────────────┐
                              │      GameManager       │  ◄── Game Engine Loop
                              └─────┬────────────┬─────┘      State & Timers
                                    │            │
             ┌──────────────────────┴┐          ┌┴──────────────────────┐
             │       screen          │          │      Console          │
             │   (Map & Lighting)    │          │ (Terminal Abstraction)│
             └──────┬────────────┬───┘          └───────────────────────┘
                    │            │
     ┌──────────────┴─┐        ┌─┴──────────────┐
     │    Player      │        │    Obstacle    │
     │  (Input/Physics)│       │  (Co-Op Push)  │
     └────────────────┘        └────────────────┘
```

---

## 🗝️ Core Subsystems

### 1. Game Loop & State Machine (`GameManager`)
- **Main Loop**: Executes at 50ms ticks (~20 FPS) with non-blocking key polling via `_kbhit()` / `_getch()`.
- **Game States**:
  - `MainMenu`: Title screen, difficulty/color mode toggle, instructions.
  - `Gameplay`: Active tick processing, collision checking, torch light masking, time decay calculation.
  - `Paused`: Game pause overlay.
  - `GameOver`: End screen showing total team score and exit metrics.

### 2. Grid & Collision Engine (`screen` & `Point`)
- **Map Representation**: 2D char grid (`MAX_X = 80`, `MAX_Y = 25`).
- **Entity Positioning**: Immutable 2D coordinate encapsulation in `Point` struct supporting operator overloading (`==`, `!=`).
- **Collision Layer**: Grid-based query system handling entity-to-terrain and entity-to-entity collisions before applying movement deltas.

### 3. Dynamic Lighting & Fog of War System (`screen::darkMask`)
- **Light Radius**: Calculates Chebyshev / Manhattan distance visibility around the active Torch holder.
- **Occlusion**: Unlit tiles outside torch illumination are masked to `MapChar::Empty` during render passes, hiding puzzle elements and map layout until explored.

### 4. Cooperative Physics & Momentum (`Spring` & `Obstacle`)
- **Co-op Push Force (`Obstacle`)**: Calculates combined force vector:
  $$\text{CanMove} = \sum \text{Force}_{\text{pushers}} \ge \text{TotalWeight}_{\text{chain}}$$
- **Spring Compression & Kinetic Transfer (`Spring`)**:
  - Compresses position stack on player entry.
  - Transfers momentum delta to colliding player upon decompression:
  $$\vec{v}_{\text{target}} = \vec{v}_{\text{spring}}, \quad \vec{v}_{\text{source}} = 0$$

### 5. Bomb & Blast Propagation (`Bomb`)
- **Timer Queue**: Active bombs maintained in `std::vector<Bomb>` with tick decrements.
- **Destruction Radius**: Raycasts up to radius $R = 3$, destroying soft walls (`MapChar::SoftWall`) while stopping at hard walls (`MapChar::Wall`).

---

## 🎨 Design Patterns Used

1. **Façade Pattern**: `Console` class provides a simplified platform-agnostic interface over Win32 API and ANSI terminal escape sequences.
2. **Observer / Delegation**: `Player` delegates spatial validation queries back to `GameManager` and `screen`.
3. **State Pattern**: `SwitchState` and `MenuChoice` drive system state transitions.
4. **Data-Driven Level Design**: Maps and trivia riddles are loaded dynamically from plain-text assets (`.screen` files and `riddles.txt`).

---

## 🛠️ Build & Toolchain Specifications

- **Language Standard**: Modern C++20 (`std:c++20`)
- **Build Generator**: CMake 3.16+
- **Supported Compilers**: MSVC 2022 (Windows), GCC 11+ (Linux), Clang 13+ (macOS)
