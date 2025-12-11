# MML Parametric Curve 2D Visualizer (FLTK)

Cross-platform visualizer for 2D parametric curves using FLTK.

## Features

- Load and visualize 2D parametric curves
- Support for multiple curves simultaneously
- Interactive legend
- Cross-platform (Windows, Linux, macOS)

## Data Format

The visualizer expects text files with the following format:

```
PARAMETRIC_CURVE_CARTESIAN_2D
<title>
t1: <min_parameter_value>
t2: <max_parameter_value>
NumPoints: <number_of_points>
<t1> <x1> <y1>
<t2> <x2> <y2>
...
```

Example:
```
PARAMETRIC_CURVE_CARTESIAN_2D
Log spiral
t1: 0
t2: 10
NumPoints: 101
0 1 0
0.1 0.737117 0.0739584
0.2 0.537872 0.109032
...
```

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

Run with command line arguments to load files at startup:
```bash
./MML_ParametricCurve2D_Visualizer curve1.txt curve2.txt
```

Or use the "Load File" button to load files interactively.
