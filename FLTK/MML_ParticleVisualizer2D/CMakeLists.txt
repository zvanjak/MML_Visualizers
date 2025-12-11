cmake_minimum_required(VERSION 3.15)
project(MML_ParticleVisualizer2D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FLTK
find_package(FLTK REQUIRED)

# Include directories
include_directories(${FLTK_INCLUDE_DIR})

# Source files
set(SOURCES
    main.cpp
    SimulationWidget.cpp
    LegendWidget.cpp
    MMLFileParser.cpp
    MMLData.cpp
)

set(HEADERS
    MMLData.h
    SimulationWidget.h
    LegendWidget.h
    MMLFileParser.h
)

# Create executable
add_executable(MML_ParticleVisualizer2D ${SOURCES} ${HEADERS})

# Link FLTK libraries
target_link_libraries(MML_ParticleVisualizer2D ${FLTK_LIBRARIES})

# On Windows, link against necessary system libraries
if(WIN32)
    target_link_libraries(MML_ParticleVisualizer2D 
        comctl32 
        wsock32 
        ws2_32
        ole32
        uuid
        gdi32
        gdiplus
    )
endif()

# Set output directory
set_target_properties(MML_ParticleVisualizer2D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
