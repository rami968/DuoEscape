# DuoEscape - Game Guide & Level Walkthrough

Welcome to **DuoEscape**, a 2-Player Cooperative ASCII Puzzle Adventure game written in C++!

---

## 🎮 Game Legend & Symbol Map

| Symbol | Object | Description |
|:------:|:-------|:------------|
| `$` | **Player 1** | Controlled using `W` (Up), `A` (Left), `S` (Down), `D` (Right), `E` (Bomb) |
| `&` | **Player 2** | Controlled using `I` (Up), `J` (Left), `K` (Down), `L` (Right), `O` (Bomb) |
| `K` | **Key** | Collected to unlock doors |
| `\` | **Switch (Off)** | Walk over to toggle to `/` (On) |
| `/` | **Switch (On)** | Active puzzle state |
| `!` | **Torch** | Illuminates dark rooms and lifts fog of war |
| `@` | **Bomb** | Explosive item used to destroy soft walls (`w`) |
| `#` | **Spring** | Compress and release to launch players across gaps |
| `*` | **Obstacle** | Heavy obstacle requiring **both players** to push together |
| `^` | **Star** | Collectible bonus points (+20 pts) |
| `W` | **Hard Wall** | Indestructible barrier |
| `w` | **Soft Wall** | Destructible barrier (explodes when hit by bomb blast) |
| `?` | **Riddle** | Trivia puzzle prompt (+30 score on success, -10 on penalty) |

---

## 🕹️ Controls & Keybindings

| Action | Player 1 (`$`) | Player 2 (`&`) |
|:-------|:--------------:|:--------------:|
| Move Up | `W` | `I` |
| Move Down | `S` | `K` |
| Move Left | `A` | `J` |
| Move Right | `D` | `L` |
| Place Bomb | `E` | `O` |
| Stay/Stop | `X` | `M` |

- **Pause Game**: Press `ESC`
- **Restart Game**: Press `R`

---

## 🏆 Scoring & Physics Mechanics

### Score System
- **Team Score**: Shared combined score between Player 1 and Player 2.
- **Riddles**: Correct answer yields **+30 points**, incorrect answer subtracts **10 points**.
- **Stars (`^`)**: Worth **+20 points** each. Positioned in challenging locations requiring bomb or push physics.
- **Fast Exit Bonus**: Completing a room under **1,800 game ticks** (~90 seconds) awards **+15 points**.
- **Time Penalty**: After **3,000 ticks**, the team loses **10 points** every **200 ticks**.

### Cooperative Push Mechanics (`*`)
- Heavy obstacles (`*`) require **both players pushing in the exact same direction simultaneously**.
- If only one player pushes, the obstacle will not budge.
- Multiple chained obstacles accumulate weight, requiring higher combined pushing force!

### Spring Physics (`#`)
- Stepping onto a spring (`#`) compresses it.
- Releasing launches the player across obstacles or gaps.
- If a launched player collides with another player mid-air, momentum and kinetic energy are transferred to the second player!

### Bombs & Destructible Terrain (`@` / `w`)
- Bombs detonate after **10 game ticks**.
- Blast radius is **3 grid units**.
- Soft walls (`w`) shatter if adjacent to blast.
- Hard walls (`W`) and external map bounds are immune to explosions.
- **Health**: Each player has **3 lives**. Being caught in an explosion loses 1 life.

---

## 🗺️ Level Walkthroughs

### Room 1: The Prisoners' Cell
- **Initial Setup**: Player 1 (`$`) starts trapped in the lower-left cell behind Door 1. Player 2 (`&`) starts free.
- **Door 1 Requirements**: Requires Key 1 at position `(32, 19)`. Player 2 must navigate to collect this key and release Player 1.
- **Door 2 Requirements**: Requires 2 Keys (`(3,16)` and `(1,8)`).
- **Door 3 (Exit)**: Requires 2 Switches (Switches 0 & 1) turned ON, plus 2 Keys (`(47,13)` and `(54,7)`).
- **Strategy**: Player 2 frees Player 1. Work together to activate switches and clear destructible soft walls (`w`) using bombs (`@`). Pick up the Torch (`!`) to light the eastern chamber.

### Room 2: The Dark Labyrinth
- **Darkness Mechanic**: The entire room is submerged in darkness (Fog of War). One player must carry the Torch (`!`) to illuminate the path for both team members.
- **Door 4**: Unlocked by Key at `(63,9)`.
- **Door 5 (Exit)**: Requires 2 Switches (`(48,21)` and `(52,15)`) activated, plus Key at `(33,9)`. Use springs (`#`) and pushable blocks (`*`) strategically.

### Room 3: Split Chambers
- **Parallel Action**: The room features a strict divide. Player 1 and Player 2 are separated on opposite sides.
- **Interdependent Switches**: Switch 5 at `(20,23)` opens Door 6 for Player 2, while Switch 4 at `(57,23)` opens Door 7 for Player 1.
- **Strategy**: Coordinate movements to unlock each other's path, collect both keys at opposite ends, and enter the main exit Door 8 `(39,19)`.
