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

# Platform-specific settings
if(WIN32)
    # Windows: link against necessary system libraries
    target_link_libraries(MML_ParticleVisualizer2D 
        comctl32 
        wsock32 
        ws2_32
        ole32
        uuid
        gdi32
        gdiplus
    )
elseif(APPLE)
    # macOS: link required frameworks
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(MML_ParticleVisualizer2D 
        ${COCOA_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    # Set macOS-specific properties for app bundle (optional)
    set_target_properties(MML_ParticleVisualizer2D PROPERTIES
        MACOSX_RPATH ON
    )
else()
    # Linux: link pthread and math libraries
    target_link_libraries(MML_ParticleVisualizer2D 
        pthread
        m
    )
endif()

# Platform-specific binary naming
if(WIN32)
    set_target_properties(MML_ParticleVisualizer2D PROPERTIES
        OUTPUT_NAME "MML_ParticleVisualizer2D_FLTK"
    )
elseif(UNIX AND NOT APPLE)
    set_target_properties(MML_ParticleVisualizer2D PROPERTIES
        OUTPUT_NAME "MML_ParticleVisualizer2D_FLTK"
    )
# macOS keeps standard name
endif()

# Set output directory
set_target_properties(MML_ParticleVisualizer2D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
