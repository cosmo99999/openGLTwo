#!/bin/bash
 conan install . --build=missing --profile:build=GL_for_linux --profile:host=GL_for_windows
