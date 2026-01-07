rem Use this batch file to build RoarEngine for Visual Studio
rmdir /s /q build
cmake -G "Visual Studio 17 2022" -A x64 -B build
