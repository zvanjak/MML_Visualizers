# MML Parametric Curve 2D Visualizer (Qt + OpenGL)

2D parametric curve visualization tool using Qt 6 and OpenGL.

## Features

- **Multi-curve support**: Load and display multiple parametric curves simultaneously
- **Interactive controls**: Pan (click + drag) and zoom (mouse wheel)
- **Color-coded curves**: Up to 10 distinct curve colors
- **Legend**: Visual curve identification
- **Data info**: View curve statistics (points, ranges)

## Building

### Windows

```cmd
cd Qt\MML_ParametricCurve2D_Visualizer
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Linux

```bash
cd Qt/MML_ParametricCurve2D_Visualizer
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Running

### Windows

```cmd
.\build\bin\Release\MML_ParametricCurve2D_Visualizer.exe ..\..\data\ParametricCurve2D\damped_harmonic_oscillator_phase_space_exact.txt
```

### Linux

```bash
./build/bin/MML_ParametricCurve2D_Visualizer_Qt ../../data/ParametricCurve2D/damped_harmonic_oscillator_phase_space_exact.txt
```

Multiple files can be specified as arguments to load multiple curves.

## Data Format

Input files use `PARAMETRIC_CURVE_CARTESIAN_2D` format:

```
PARAMETRIC_CURVE_CARTESIAN_2D
Title
t1 <min_parameter>
t2 <max_parameter>
NumPoints <count>
<t> <x> <y>
<t> <x> <y>
...
```

Example data files are in `data/ParametricCurve2D/`.

## Controls

- **Left click + drag**: Pan view
- **Mouse wheel**: Zoom in/out
- **Load Curve button**: Add new curve
- **Reset View button**: Fit all curves to viewport

## Dependencies

- Qt 6.x (Core, Gui, Widgets, OpenGLWidgets)
- OpenGL
- C++17 compiler
