# MML Vector Field 2D Visualizer (FLTK)

Cross-platform visualizer for 2D vector fields using FLTK.

## Features

- Load and visualize 2D vector fields
- Automatic vector scaling
- Coordinate system with grid
- Display vector count and max magnitude
- Cross-platform (Windows, Linux, macOS)

## Data Format

The visualizer expects text files with the following format:

```
VECTOR_FIELD_2D_CARTESIAN
<title>
<position_x> <position_y> <vector_x> <vector_y>
<position_x> <position_y> <vector_x> <vector_y>
...
```

Example:
```
VECTOR_FIELD_2D_CARTESIAN
rotational_field
-10 -10 10 -10
-10 -8 8 -10
-10 -6 6 -10
-8 -10 10 -8
-8 -8 8 -8
...
```

Each line after the title contains four space-separated values:
- Position X coordinate
- Position Y coordinate
- Vector X component
- Vector Y component

## Building

### Prerequisites
- CMake 3.15 or higher
- FLTK library
- C++17 compatible compiler

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

Run with a data file:
```bash
./MML_VectorField2D_Visualizer vector_field.txt
```

Or use the "Load File" button to load files interactively.

### Features
- Vectors are automatically scaled based on field dimensions
- Arrow heads indicate vector direction
- Grid and axes provide spatial reference
- Info bar shows number of vectors and maximum magnitude
