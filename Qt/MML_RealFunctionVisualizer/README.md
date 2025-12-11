# MML Real Function Visualizer (Qt + OpenGL)

Cross-platform 2D real function visualizer using Qt 6 and OpenGL. Third implementation after WPF and FLTK versions.

## Features

- **Multiple Functions**: Load and display multiple real functions simultaneously
- **Interactive Controls**: 
  - Pan: Left/Right mouse button + drag
  - Zoom: Mouse wheel
  - Reset View: Auto-fit all functions
- **OpenGL Rendering**: Hardware-accelerated 2D graphics
- **Modern UI**: Qt Widgets interface with function legend
- **Real-time Information**: Display function statistics (points, ranges)
- **Color Coding**: Automatic color assignment for each function

## Building

### Prerequisites

- **Qt 6.10.0+**: Installed at `C:/Dev/Qt/6.10.0/msvc2022_64`
- **CMake 3.16+**
- **Visual Studio 2022** (or compatible C++17 compiler)
- **OpenGL**: Typically included with graphics drivers

### Build Steps

#### Windows (Automated)

```bash
build.bat
```

This will:
1. Create build directory
2. Configure with CMake
3. Build Release configuration
4. Display deployment instructions

#### Manual Build

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Deploy Qt DLLs (Windows)

After building, deploy required Qt DLLs:

```bash
C:\Dev\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe build\bin\Release\MML_RealFunctionVisualizer.exe
```

## Usage

### Command Line

```bash
# No arguments - opens empty window
.\MML_RealFunctionVisualizer.exe

# Load single function
.\MML_RealFunctionVisualizer.exe path/to/function.txt

# Load multiple functions
.\MML_RealFunctionVisualizer.exe func1.txt func2.txt func3.txt
```

### Interactive Controls

- **Load Function Button**: Open file dialog to add more functions
- **Reset View Button**: Auto-fit all loaded functions
- **Left Mouse + Drag**: Pan the view
- **Right Mouse + Drag**: Pan the view
- **Mouse Wheel**: Zoom in/out

## Data Format

Reads `REAL_FUNCTION_DATA` format from WPF visualizers:

```
REAL_FUNCTION_DATA
Function Title
NumPoints
xMin xMax
x1 y1
x2 y2
...
```

Example files in `../../WPF/MML_RealFunctionVisualizer/data/`:
- `cosh.txt` - Hyperbolic cosine
- `sinh.txt` - Hyperbolic sine
- `sin.txt` - Sine function
- `cos.txt` - Cosine function

## Architecture

### Core Components

- **MainWindow**: Qt Widgets UI with controls and info display
- **GLWidget**: QOpenGLWidget for 2D OpenGL rendering
- **MMLData**: Data structures (Point2D, LoadedRealFunction, Color)
- **MMLFileParser**: Parser for REAL_FUNCTION_DATA format

### Key Features

- 2D orthographic projection with aspect ratio preservation
- Coordinate transformation (world ↔ screen)
- Interactive mouse controls (pan/zoom)
- Automatic bounds calculation
- Grid and axis rendering

## Testing

Test with sample data:

```bash
cd build\bin\Release

# Single function
.\MML_RealFunctionVisualizer.exe ..\..\..\..\..\WPF\MML_RealFunctionVisualizer\data\cosh.txt

# Multiple functions
.\MML_RealFunctionVisualizer.exe ^
    ..\..\..\..\..\WPF\MML_RealFunctionVisualizer\data\cosh.txt ^
    ..\..\..\..\..\WPF\MML_RealFunctionVisualizer\data\sinh.txt ^
    ..\..\..\..\..\WPF\MML_RealFunctionVisualizer\data\sin.txt
```

## Comparison with Other Implementations

### WPF Version
- Platform: Windows only (C#/XAML)
- Graphics: WPF rendering
- Status: Reference implementation

### FLTK Version
- Platform: Cross-platform (C++/FLTK)
- Graphics: FLTK 2D drawing
- Status: Complete and tested

### Qt Version (This)
- Platform: Cross-platform (C++/Qt)
- Graphics: OpenGL hardware acceleration
- Advantages:
  - Modern UI framework
  - Hardware-accelerated rendering
  - Better scaling for complex visualizations
  - Professional look and feel

## Dependencies

- Qt6::Core
- Qt6::Gui
- Qt6::Widgets
- Qt6::OpenGLWidgets
- OpenGL (opengl32.lib on Windows)

## Known Issues

- HTML color format warnings in console (harmless, Qt QTextHtmlParser quirk)
- No text rendering in OpenGL viewport yet (future enhancement)

## Future Enhancements

- [ ] Export view as image (PNG, JPG)
- [ ] OpenGL text rendering for axis labels
- [ ] Animation support for time-varying functions
- [ ] Custom color selection
- [ ] Function expression evaluation
- [ ] Measurement tools (distance, value at point)

## License

Part of MML_Visualizers project.

## See Also

- FLTK version: `../../FLTK/MML_RealFunctionVisualizer/`
- WPF version: `../../WPF/MML_RealFunctionVisualizer/`
- Qt 3D curves: `../MML_ParametricCurve3D_Visualizer/`
