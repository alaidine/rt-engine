rem Use this batch file to build RoarEngine for Visual Studio
rmdir /s /q build
conan install . --output-folder=build --build=missing -s compiler.cppstd=20 -s build_type=Debug
cmake --preset conan-default
