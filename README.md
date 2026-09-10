# C++ Project: DuoEscape 🎮
### *2-Player Cooperative ASCII Puzzle Adventure Engine in C++20*

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg?style=flat-square&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C.svg?style=flat-square&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg?style=flat-square)
![CI Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=flat-square)

---

```
  ____               _____                               
 |  _ \  _   _  ___ | ____| ___  ___  __ _ _ __   ___    
 | | | || | | |/ _ \|  _|  / __|/ __|/ _` | '_ \ / _ \   
 | |_| || |_| | (_) | |___ \__ \ (__| (_| | |_) |  __/   
 |____/  \__,_|\___/|_____||___/\___|\__,_| .__/ \___|   
                                          |_|            
```

**DuoEscape** is a feature-rich, 2-player cooperative 2D ASCII puzzle game engine written in modern C++20. Designed with custom physics, real-time collision detection, dynamic line-of-sight lighting, destructible environments, and interactive puzzle mechanisms, two players must synchronize their actions to escape multi-chambered labyrinths.

---

## 🌟 Key Features

- 🤝 **Cooperative Gameplay Mechanics**: Built from the ground up for 2 players ($`$` and `&`$) requiring real-time teamwork, shared strategy, and synchronized movement.
- 💡 **Dynamic Lighting & Fog-of-War System**: Dark chambers submerge players in obscurity unless a player carries a **Torch** (`!`), projecting dynamic line-of-sight illumination across the screen.
- 🏋️ **Co-Op Heavy Push Physics**: Heavy obstacles (`*`) calculate mass and friction vectors, moving **only** when both players apply force in the exact same direction simultaneously.
- 🚀 **Spring Kinetic Energy Transfer**: Springs (`#`) store compressive potential energy, launching players across gaps and transferring kinetic momentum upon mid-air collisions.
- 💣 **Destructible Terrain & Bomb Timers**: Place bombs (`@`) with 10-tick countdowns and 3-tile blast radiuses to blast through soft walls (`w`).
- 🧩 **Multi-Chamber Puzzle Engine**: Interconnected doors, key cards (`K`), switchboards (`/`, `\`), and trivia riddles (`?`) controlling door locks and stage exits.
- 🎨 **Platform-Agnostic Console Rendering**: Custom terminal abstraction (`Console`) handling color attributes, cursor positioning, and ANSI escape code rendering.
- 🛠️ **Modern CMake Build System**: Full support for cross-platform compilation with C++20 standard enforcement.

---

## 🎮 Gameplay Controls

| Action | Player 1 (`$`) | Player 2 (`&`) |
|:-------|:--------------:|:--------------:|
| Move Up | <kbd>W</kbd> | <kbd>I</kbd> |
| Move Down | <kbd>S</kbd> | <kbd>K</kbd> |
| Move Left | <kbd>A</kbd> | <kbd>J</kbd> |
| Move Right | <kbd>D</kbd> | <kbd>L</kbd> |
| Plant Bomb | <kbd>E</kbd> | <kbd>O</kbd> |
| Stay / Stop | <kbd>X</kbd> | <kbd>M</kbd> |

- **Pause Game**: <kbd>ESC</kbd>
- **Restart Level**: <kbd>R</kbd>

---

## 🖥️ Gameplay Overview

```text
================================================================================
| WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW |
| W $   w                    W               K             !                 W |
| W     w   *                W   WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    W |
| W     w                    W   W                                      W    W |
| W11111w                    W   W   &                  ^               W    W |
| WWWWWWWWWWWWWWWWWWWWWWWWWWWW   WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    W |
| W                                                                          W |
| WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW |
================================================================================
| SCORE: 050  | LIVES: P1[3] P2[3] | SCREEN: Room 1 | TORCH: ON | TIME: 0142s   |
================================================================================
```

---

## ⚙️ Building & Running

### Prerequisites
- **Compiler**: MSVC (Visual Studio 2022+), GCC 10+, or Clang 12+ (C++20 support required)
- **Build System**: [CMake 3.16+](https://cmake.org/download/)

### Quickstart (CMake)

```bash
# 1. Clone the repository
git clone https://github.com/rami968/DuoEscape.git
cd DuoEscape

# 2. Configure project with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build executable
cmake --build build --config Release

# 4. Run DuoEscape
./build/Release/DuoEscape.exe   # On Windows
./build/DuoEscape              # On Linux/macOS
```

### Visual Studio 2022
1. Open Visual Studio.
2. Select **Open a Local Folder** and choose the `DuoEscape` repository root directory.
3. Visual Studio will automatically detect `CMakeLists.txt`.
4. Select `DuoEscape.exe` from the target dropdown and click **Run (F5)**.

---

## 📁 Repository Structure

```
DuoEscape/
├── CMakeLists.txt              # Cross-platform CMake build configuration
├── README.md                   # Project overview & documentation
├── LICENSE                     # MIT License
├── .gitignore                  # Git ignore rules for C++ & IDEs
├── include/                    # Header files (.h)
│   ├── Console.h               # Terminal rendering abstraction
│   ├── GameManager.h           # Main engine loop & state controller
│   ├── Player.h                # Player entity logic & controls
│   ├── screen.h                # Grid rendering & light-masking engine
│   └── ...                     # Subsystem header definitions
├── src/                        # Implementation files (.cpp)
│   ├── Console.cpp
│   ├── GameManager.cpp
│   ├── main.cpp
│   └── ...
├── assets/                     # Level maps (*.screen) & riddles text file
├── docs/                       # Technical architecture & walkthrough guides
│   ├── ARCHITECTURE.md         # System design & OOP patterns guide
│   └── GAME_GUIDE.md           # Full game guide & level solution manual
└── .github/
    └── workflows/
        └── ci.yml              # Automated GitHub Actions CI workflow
```

---

## 📐 Architecture & Documentation

- 📄 For an in-depth breakdown of class designs, state machine transitions, dynamic lighting algorithms, and physics formulas, see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).
- 📜 For detailed level maps, door unlock requirements, and item mechanics, see [docs/GAME_GUIDE.md](docs/GAME_GUIDE.md).

---

## 📜 License

Distributed under the **MIT License**. See `LICENSE` for more information.

---

## 👨‍💻 Author

Developed by **Rami Zargian**. Feel free to reach out or connect on LinkedIn!
