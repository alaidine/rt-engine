
## Getting Started

#### Linux
When setting up this template on linux for the first time, install the dependencies from this page:
([Working on GNU Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux))

You can use this templates in a few ways: using Visual Studio, using CMake, or make your own build setup. This repository comes with Visual Studio and CMake already set up.

### CMake

- Type the follow command:

```sh
cmake -S . -B build
```

> if you want to configure your project to build with debug symbols, use the flag `-DCMAKE_BUILD_TYPE=Debug`

- After CMake configures your project, build with:

```sh
cmake --build build
```

- Inside the build folder is another folder (named the same as the project name on CMakeLists.txt) with the executable and resources folder.
- In order for resources to load properly, cd to `src` and run the executable (`../build/${PROJECT_NAME}/${PROJECT_NAME}`) from there.

- cmake will automatically download a current release of raylib but if you want to use your local version you can pass `-DFETCHCONTENT_SOURCE_DIR_RAYLIB=<dir_with_raylib>` 

## R-Type

![alt text](logo/r-type.jpg "R-Type")

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
