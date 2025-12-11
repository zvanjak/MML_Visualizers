################################################################################
# MML Visualizers - FLTK Implementation
# CMakeLists.txt for building all FLTK-based visualizers
#
# Cross-platform build system for Linux, Windows, and macOS
################################################################################

cmake_minimum_required(VERSION 3.15)
project(MML_Visualizers_FLTK VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

################################################################################
# Find FLTK package
################################################################################

find_package(FLTK REQUIRED)

if(NOT FLTK_FOUND)
    message(FATAL_ERROR "FLTK not found. Please install FLTK or set FLTK_DIR.")
endif()

# Include FLTK directories
include_directories(${FLTK_INCLUDE_DIR})

################################################################################
# Platform-specific settings
################################################################################

if(WIN32)
    # Windows-specific settings
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    
    # Additional Windows libraries needed by FLTK
    set(PLATFORM_LIBS
        comctl32
        wsock32
        ws2_32
        ole32
        uuid
        gdi32
        gdiplus
    )
    
    # Set subsystem to WINDOWS for GUI applications
    if(MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    
elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    set(PLATFORM_LIBS
        pthread
        dl
        m
    )
    
elseif(APPLE)
    # macOS-specific settings
    set(PLATFORM_LIBS)
    set(CMAKE_MACOSX_RPATH ON)
    
endif()

################################################################################
# Output directories
################################################################################

# Set output directory for all executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Create bin directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

################################################################################
# Compiler flags
################################################################################

if(MSVC)
    # MSVC-specific flags
    add_compile_options(/W4)
    add_compile_options(/utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # GCC/Clang flags
    add_compile_options(-Wall -Wextra -pedantic)
endif()

################################################################################
# Add subdirectories for each visualizer
################################################################################

add_subdirectory(MML_RealFunctionVisualizer)
add_subdirectory(MML_ParametricCurve2D_Visualizer)
add_subdirectory(MML_ParticleVisualizer2D)
add_subdirectory(MML_VectorField2D_Visualizer)

################################################################################
# Installation rules (optional)
################################################################################

# Install executables
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        DESTINATION bin
        FILES_MATCHING PATTERN "*"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ)

# Install data files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/../data/
        DESTINATION share/MML_Visualizers/data
        FILES_MATCHING PATTERN "*.txt")

################################################################################
# Build summary
################################################################################

message(STATUS "==============================================")
message(STATUS "MML Visualizers FLTK Build Configuration")
message(STATUS "==============================================")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "----------------------------------------------")
message(STATUS "FLTK Configuration:")
message(STATUS "  Include Dir: ${FLTK_INCLUDE_DIR}")
message(STATUS "  Libraries: ${FLTK_LIBRARIES}")
message(STATUS "----------------------------------------------")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "----------------------------------------------")
message(STATUS "Building visualizers:")
message(STATUS "  - MML_RealFunctionVisualizer")
message(STATUS "  - MML_ParametricCurve2D_Visualizer")
message(STATUS "  - MML_ParticleVisualizer2D")
message(STATUS "  - MML_VectorField2D_Visualizer")
message(STATUS "==============================================")
