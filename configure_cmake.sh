#!/bin/bash

rm -rdf build/
conan install . --output-folder=build --build=missing -s compiler.cppstd=20 -s build_type=Debug
cmake --preset conan-debug
