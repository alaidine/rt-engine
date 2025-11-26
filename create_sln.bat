@echo off
if exist "build\" (
    echo Build directory exists, building project...
    cmake --build build
) else (
    echo Build directory not found, configuring and building project...
    cmake -B build/ -S . -DFETCHCONTENT_SOURCE_DIR_RAYLIB=../raylib -DFETCHCONTENT_SOURCE_DIR_SOL2=../sol2 -DFETCHCONTENT_SOURCE_DIR_BOX2D=../box2d
    cmake --build build
)
