cmake_minimum_required(VERSION 3.10)
project(ProtocolStackSimulation
    VERSION 1.0.0
    DESCRIPTION "OSI Protocol Stack Simulation in C++17"
    LANGUAGES CXX
)

# ── C++17 standard ────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Compiler warnings ────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Include directory ─────────────────────────────────────────────────────
include_directories(${PROJECT_SOURCE_DIR}/include)

# ── Source files ──────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/network_layer.cpp
    src/datalink_layer.cpp
    src/physical_layer.cpp
    src/medium.cpp
    src/node.cpp
)

# ── Executable ────────────────────────────────────────────────────────────
add_executable(protocol_sim ${SOURCES})
