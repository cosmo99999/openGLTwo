#!/bin/bash
cmake --preset conan-default
cmake --list-presets        
cmake --build --preset conan-release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
