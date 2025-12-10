# bluepill_power_monitoring

Converting the STM32 Project from C to C++ (Using CMake Only)


1. Add Your C++ Source File

Place your .cpp file inside the project, e.g.:

Core/Src/power.cpp
Core/Inc/power.h

2. Enable C++ in CMake

Ensure the top-level CMakeLists.txt contains:

enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

3. Add the C++ File to the Build

Inside target_sources():

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/power.cpp
)

4. Add Include Path

Make sure the header path is included:

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
)

5. Reconfigure and Build