# Pong Networking — Course Project

Project: `pong-networking`

## Overview

- Game chosen: **Pong** (classic 2D paddle-and-ball game).
- Purpose: course project for Networking for Games. The repository contains a baseline single-machine Pong implementation and places to extend it with networking (authoritative server, client-server, or peer-to-peer) for the assignment.

## Baseline game summary

The baseline implements a minimal Pong clone with the following features:
- Two paddles (left and right) controlled by keyboard input.
- A ball with simple linear motion and basic collision response against paddles and top/bottom walls.
- Score tracking and round reset when a player concedes a point.
- Deterministic fixed-step update loop (input -> update physics -> collision -> render).

### How the baseline game works (short)

1. Initialization
   - Create a window and renderer.
   - Initialize game objects: two paddles, one ball, and score counters.
2. Game loop (fixed-step)
   - Poll and process input (player controls move paddles up/down).
   - Update game state (move paddles, update ball position using velocity).
   - Collision detection & response (ball vs paddle, ball vs top/bottom walls).
   - Score handling: when the ball passes a left or right goal line, increment opponent's score and reset ball and paddle positions.
   - Render the current game state and present the frame.
3. Shutdown
   - Free resources and exit cleanly.

## High-level architecture & flow

The ASCII diagram below shows the high-level runtime flow of the game application:

```
Start
  |
  v
Initialization -------------------------------
  |  - Create Window/Renderer                   |
  |  - Init paddles, ball, scores               |
  v                                            v
Game Loop (fixed-step) ----------------------> Render
  |                                             |
  |---> Input (keyboard)                        |
  |---> Update Physics & Game State             |
  |       - Move paddles based on input         |
  |       - Move ball based on velocity         |
  |       - Collision detection & response      |
  |---> Score check & round reset               |
  |                                             v
  +-----------------------------------------> Present frame
  |
  v
Shutdown
```

Notes on networking (course extension)

- Typical networking strategies for Pong:
  - Authoritative server: server simulates the game, clients send inputs. Server resolves collisions and authoritative state, then sends periodic state snapshots to clients.
  - Client-side prediction + interpolation: clients predict local results for low latency and interpolate server updates for remote objects.
  - Peer-to-peer: peers exchange inputs and reconcile state; usually requires rollback or strict timing to stay in sync.
- Look for folders or files named `server`, `client`, `network`, `net`, or `protocol` for existing networking-related code.

## Build instructions (Windows / Visual Studio)

Prerequisites
- Windows 10 or later with Visual Studio 2019 or 2022.
- Install the "Desktop development with C++" workload in the Visual Studio Installer.
- (Optional) Git command-line client.

Steps to build from a fresh clone

1. Clone the repository

   `git clone https://github.com/OlawoleAbayomi-Owodunni/pong-networking.git`
   `cd "pong-networking"`

2. Open the project/solution in Visual Studio

   - If a solution file exists (for example `Pong.sln`), open it: `File -> Open -> Project/Solution -> Pong.sln`.
   - Otherwise open the project file: `File -> Open -> Project/Solution -> Pong\Pong.vcxproj`.

3. Configure project settings (if necessary)

   - In Solution Explorer, right-click the project and select `Properties`.
   - Under `Configuration Properties -> C/C++ -> Language`, set `C++ Language Standard` to `ISO C++17 (/std:c++17)`.
   - Choose `Configuration` (Debug/Release) and `Platform` (Win32/x64) appropriate for your environment.

4. Build the project

   - Use `Build -> Build Solution` or press `Ctrl+Shift+B`.
   - The executable will typically be created in `Pong\Debug\` or `Pong\Release\` depending on configuration.

5. Run

   - From Visual Studio: press `F5` to run with debugger, or `Ctrl+F5` to run without debugging.
   - Or run the produced executable from the output folder.
