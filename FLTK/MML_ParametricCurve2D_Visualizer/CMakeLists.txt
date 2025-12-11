cmake_minimum_required(VERSION 3.15)
project(MML_ParametricCurve2D_Visualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FLTK
find_package(FLTK REQUIRED)

# Include directories
include_directories(${FLTK_INCLUDE_DIR})

# Source files
set(SOURCES
    main.cpp
    GraphWidget.cpp
    LegendWidget.cpp
    MMLFileParser.cpp
)

set(HEADERS
    MMLData.h
    GraphWidget.h
    LegendWidget.h
    MMLFileParser.h
)

# Create executable
add_executable(MML_ParametricCurve2D_Visualizer ${SOURCES} ${HEADERS})

# Link FLTK libraries
target_link_libraries(MML_ParametricCurve2D_Visualizer ${FLTK_LIBRARIES})

# On Windows, link against necessary system libraries
if(WIN32)
    target_link_libraries(MML_ParametricCurve2D_Visualizer 
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
set_target_properties(MML_ParametricCurve2D_Visualizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
