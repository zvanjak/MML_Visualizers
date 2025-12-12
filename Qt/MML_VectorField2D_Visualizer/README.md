# MML Vector Field 2D Visualizer - Qt Version

A cross-platform 2D vector field visualizer built with Qt 6 and OpenGL, designed to display and analyze vector fields with interactive controls.

## Features

- **Arrow-Based Rendering**: Displays vectors as arrows at grid points
- **Magnitude Color Mapping**: Color gradient from blue (low) to red (high magnitude)
- **Arrow Scaling**: Adjustable arrow length via slider
- **Vector Normalization**: Option to show direction only (equal-length arrows)
- **Interactive View**: Pan (mouse drag) and zoom (mouse wheel)
- **Customizable Display**: Toggle magnitude coloring and normalization
- **Information Panel**: Shows rendering options and controls

## Building

### Prerequisites

- CMake 3.16 or higher
- Qt 6.10.0 or higher
- C++17 compatible compiler
- OpenGL support

### Windows with Visual Studio 2022

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

Or use the provided build script:
```bash
build.bat
```

### Linux

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..
cmake --build . --config Release
```

Binary will be named `MML_VectorField2D_Visualizer_Qt` on Linux.

### macOS

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/macos ..
cmake --build . --config Release
```

## Deployment

### Windows

After building, deploy Qt dependencies:

```bash
cd build/Release
C:/Dev/Qt/6.10.0/msvc2022_64/bin/windeployqt.exe MML_VectorField2D_Visualizer.exe
```

### Linux

```bash
export LD_LIBRARY_PATH=/path/to/Qt/6.x.x/gcc_64/lib:$LD_LIBRARY_PATH
./MML_VectorField2D_Visualizer_Qt
```

## Usage

### GUI Mode

```bash
./MML_VectorField2D_Visualizer
```

Use "Load Data File..." button to select a vector field data file.

### Command-Line Mode

```bash
./MML_VectorField2D_Visualizer path/to/datafile.txt
```

## Data File Format

The visualizer reads `VECTOR_FIELD_2D_CARTESIAN` format files:

```
VECTOR_FIELD_2D_CARTESIAN
Vector Field Title
px1 py1 vx1 vy1
px2 py2 vx2 vy2
px3 py3 vx3 vy3
...
```

### Format Details

- Line 1: Format identifier "VECTOR_FIELD_2D_CARTESIAN"
- Line 2: Vector field title/description
- Remaining lines: Vector data (position_x position_y vector_x vector_y)

## Rendering Controls

### Arrow Scale Slider
- Range: 0.1 to 2.0
- Default: 0.5
- Adjusts the visual length of arrows

### Normalize Vectors
- Unchecked (default): Arrow length represents magnitude
- Checked: All arrows same length (direction only)

### Color by Magnitude
- Checked (default): Color gradient blue → cyan → green → yellow → red
- Unchecked: All arrows black

## Mouse Controls

- **Left Mouse Drag**: Pan the view
- **Mouse Wheel**: Zoom in/out

## Color Gradient

When "Color by Magnitude" is enabled:
- **Blue**: Lowest magnitude (0%)
- **Cyan**: Low magnitude (25%)
- **Green**: Medium magnitude (50%)
- **Yellow**: High magnitude (75%)
- **Red**: Highest magnitude (100%)

## Data Files

Sample vector field data files are located in:
```
../../data/VectorField2D/
../../data/VectorField2D/Basic/
```

## Architecture

- **Main Components**:
  - `MainWindow`: Qt GUI with rendering controls
  - `GLWidget`: OpenGL rendering widget with arrow drawing
  - `MMLFileParser`: Data file parsing
  - `MMLData`: Data structures (Vector2D, VectorRepr, VectorField2D)

- **Rendering**: OpenGL 2D with color-mapped arrows
- **View System**: Orthographic projection with pan/zoom

## Cross-Platform Notes

### Binary Naming
- Windows/macOS: `MML_VectorField2D_Visualizer`
- Linux: `MML_VectorField2D_Visualizer_Qt` (with suffix)

### OpenGL
- Uses OpenGL 2.1 compatible rendering
- Arrow heads drawn as filled triangles
- Color gradients for magnitude visualization

## License

Part of the MML_Visualizers project.
