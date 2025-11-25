
## Getting Started

#### Linux
When setting up this template on linux for the first time, install the dependencies from this page:
([Working on GNU Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux))

You can use this templates in a few ways: using Visual Studio, using CMake, or make your own build setup. This repository comes with Visual Studio and CMake already set up.

### Project Structure

The project is now organized into separate modules:
- **core/**: Static library containing shared functionality (networking, physics, scripting)
- **client/**: Client executable with graphics and input handling  
- **server/**: Server executable with game logic and networking
- **resources/**: Game assets (sprites, scripts, etc.)

### CMake (Recommended)

#### Basic Build:
```sh
cmake -S . -B build
cmake --build build
```

#### Development Build with Debug Symbols:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

#### Using Local Dependencies (for faster builds):
If you have `raylib`, `sol2`, and `box2d` in the parent directory:
```sh
cmake -S . -B build \
  -DFETCHCONTENT_SOURCE_DIR_RAYLIB=../raylib \
  -DFETCHCONTENT_SOURCE_DIR_SOL2=../sol2 \
  -DFETCHCONTENT_SOURCE_DIR_BOX2D=../box2d
```

#### Running:
- Executables are in `build/r-type/`
- Resources are automatically copied to the output directory

### Visual Studio (Legacy)

**Note**: Visual Studio projects are legacy and may require manual dependency setup. We recommend using CMake.

For VS2022 projects, you need local dependencies in the parent directory:
- Some parent directory
  - `raylib/` - https://github.com/raysan5/raylib  
  - `sol2/` - https://github.com/ThePhD/sol2
  - `box2d/` - https://github.com/erincatto/box2d
  - `r-type/` - This project

Then open `projects/VS2022/r-type.sln`

### Dependencies

This project uses:
- **Raylib 5.0**: Graphics and game framework
- **Sol2**: Modern C++ Lua binding library  
- **Lua 5.4**: Embedded scripting language (precompiled libraries in `lua/`)
- **Box2D 2.4.1**: 2D physics engine

**Core Library**: All shared dependencies are linked into `r-type_core` static library, which is then used by both client and server executables.

**Automatic Fetching**: CMake automatically downloads raylib, sol2, and box2d. Use `FETCHCONTENT_SOURCE_DIR_*` variables to use local copies for faster builds.

## R-Type

![alt text](resources/logo/r-type.jpg "R-Type")

### Description

Clone of the classic side-scrolling shoot 'em up arcade game R-Type, developed by Irem and released in 1987. Players control a futuristic spacecraft, the R-9, as they navigate through various levels filled with enemies and obstacles. The game is known for its challenging gameplay, unique power-up system, and iconic boss battles.

### Developers

 - Aalaaïddine ALI MLINDE
 - Mathéo MOINACHE
 - Lucas MARTIN
 - Maël MANGATA

### License

This game sources are licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

*Copyright (c) 2025 Aalaaïddine ALI MLINDE, Mathéo MOINACHE, Lucas MARTIN, Maël MANGATA All rights reserved.*
