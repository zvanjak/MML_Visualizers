cmake_minimum_required(VERSION 3.15)
project(MML_ScalarFunction2D_Visualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set output directories for all configurations
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)

qt_standard_project_setup()

set(SOURCES
    main.cpp
    MainWindow.cpp
    GLWidget.cpp
    MMLFileParser.cpp
)

set(HEADERS
    MainWindow.h
    GLWidget.h
    MMLData.h
    MMLFileParser.h
)

qt_add_executable(MML_ScalarFunction2D_Visualizer
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(MML_ScalarFunction2D_Visualizer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    OpenGL::GL
)

# Platform-specific binary naming (Linux only gets _Qt suffix)
if(UNIX AND NOT APPLE)
    set_target_properties(MML_ScalarFunction2D_Visualizer PROPERTIES
        OUTPUT_NAME "MML_ScalarFunction2D_Visualizer_Qt"
    )
endif()

# Set output directory
set_target_properties(MML_ScalarFunction2D_Visualizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows specific settings
if(WIN32)
    set_target_properties(MML_ScalarFunction2D_Visualizer PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
