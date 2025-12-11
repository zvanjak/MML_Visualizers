# MML Parametric Curve 3D Visualizer - Qt Edition

Cross-platform 3D visualizer for parametric curves using Qt and OpenGL.

## Features

- **Interactive 3D Visualization**
  - Smooth OpenGL rendering
  - Multiple curve support with color coding
  - Real-time camera controls
  
- **Camera Controls**
  - **Left Mouse Button**: Rotate camera around curves
  - **Right/Middle Mouse Button**: Pan camera
  - **Mouse Wheel**: Zoom in/out
  - **Reset Button**: Return to default view
  
- **Visual Elements**
  - Color-coded curves
  - 3D coordinate axes (X=Red, Y=Green, Z=Blue)
  - Reference grid on XY plane
  - Automatic scene bounds calculation

## Building

### Prerequisites

- Qt 6.x installed (found in C:/Dev/Qt)
- CMake 3.16 or higher
- C++17 compatible compiler (MSVC 2019+, GCC, Clang)

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (specify Qt path)
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.x.x/msvc2019_64 ..

# Build
cmake --build . --config Release

# Run
bin/Release/MML_ParametricCurve3D_Visualizer.exe
```

### Quick Build Script (Windows)

```powershell
# Assuming Qt is in C:/Dev/Qt
$QT_PATH = "C:/Dev/Qt/6.7.0/msvc2019_64"  # Adjust version as needed

mkdir build -Force
cd build
cmake -DCMAKE_PREFIX_PATH=$QT_PATH -G "Visual Studio 17 2022" ..
cmake --build . --config Release
cd ..
```

## Data Format

The visualizer reads files in the `PARAMETRIC_CURVE_CARTESIAN_3D` format:

```
PARAMETRIC_CURVE_CARTESIAN_3D
Curve Name or Description
t1: <start_parameter>
t2: <end_parameter>
NumPoints: <number_of_points>
<t1> <x1> <y1> <z1>
<t2> <x2> <y2> <z2>
...
```

### Example

```
PARAMETRIC_CURVE_CARTESIAN_3D
Helix
t1: 0
t2: 10
NumPoints: 100
0.0 1.0 0.0 0.0
0.1 0.995 0.099 0.1
0.2 0.980 0.196 0.2
...
```

## Usage

### Load Files via Command Line

```bash
# Single file
MML_ParametricCurve3D_Visualizer.exe curve.txt

# Multiple files
MML_ParametricCurve3D_Visualizer.exe curve1.txt curve2.txt curve3.txt
```

### Load Files via GUI

1. Launch the application
2. Click "Load Curve File" button
3. Select one or more `.txt` files
4. Curves will be displayed with different colors

### Example Test Data

```bash
# Run with example data from WPF
MML_ParametricCurve3D_Visualizer.exe ^
    ../../WPF/MML_ParametricCurve3D_Visualizer/data/example15_curve3D.txt ^
    ../../WPF/MML_ParametricCurve3D_Visualizer/data/body0.txt
```

## Architecture

- **MainWindow**: Main application window with UI controls
- **GLWidget**: OpenGL widget handling 3D rendering and camera
- **MMLData**: Data structures (Point3D, Color, LoadedParametricCurve3D)
- **MMLFileParser**: File parsing for PARAMETRIC_CURVE_CARTESIAN_3D format

## Features Comparison: Qt vs WPF

| Feature | Qt Version | WPF Version |
|---------|-----------|-------------|
| Platform | Cross-platform (Win/Linux/Mac) | Windows only |
| Rendering | OpenGL | WPF 3D |
| Camera Controls | Mouse-based orbit/pan/zoom | Similar |
| Performance | Excellent | Good |
| Look & Feel | Native per platform | Windows-native |

## Next Steps

This proof-of-concept can be extended to:
- Add more 3D visualizers (surfaces, vector fields, particles)
- Implement lighting and shading
- Add curve thickness control
- Export rendered images
- Animation timeline for time-dependent curves
- Measurement tools (arc length, curvature)

## License

Part of the MML_Visualizers project.
