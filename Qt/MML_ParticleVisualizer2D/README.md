# MML Particle Visualizer 2D - Qt Version

A cross-platform 2D particle simulation visualizer built with Qt 6 and OpenGL, designed to visualize particle physics simulations with animation playback.

## Features

- **Multi-Particle Rendering**: Displays multiple particles with different colors and sizes
- **Animation Playback**: Play/pause/stop controls with configurable frame rate
- **Timeline Scrubbing**: Slider to navigate through simulation timesteps
- **Step Controls**: Frame-by-frame navigation (forward/backward)
- **Interactive View**: Pan (mouse drag) and zoom (mouse wheel)
- **Statistics Panel**: Shows simulation parameters and particle information
- **Legend**: Color-coded particle list with names and radii
- **Command-Line Loading**: Load data files directly from command line

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

### Linux

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..
cmake --build . --config Release
```

Binary will be named `MML_ParticleVisualizer2D_Qt` on Linux.

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
cd build/bin/Release
C:/Dev/Qt/6.10.0/msvc2022_64/bin/windeployqt.exe MML_ParticleVisualizer2D.exe
```

### Linux

```bash
# Qt libraries should be in your LD_LIBRARY_PATH or use linuxdeployqt
export LD_LIBRARY_PATH=/path/to/Qt/6.x.x/gcc_64/lib:$LD_LIBRARY_PATH
./MML_ParticleVisualizer2D_Qt
```

## Usage

### GUI Mode

```bash
./MML_ParticleVisualizer2D
```

Use "Load Data File..." button to select a particle simulation data file.

### Command-Line Mode

```bash
./MML_ParticleVisualizer2D path/to/datafile.txt
```

## Data File Format

The visualizer reads `PARTICLE_SIMULATION_DATA_2D` format files:

```
PARTICLE_SIMULATION_DATA_2D
Width 1000
Height 800
NumBalls 3
Ball1 Red 20
Ball2 Blue 15
Ball3 Green 25
NumSteps 100
Step 0 0.0
0 100.5 200.3
1 300.2 150.7
2 500.8 400.1
Step 1 0.01
0 102.3 198.5
1 305.6 152.1
2 498.2 402.5
...
```

### Format Details

- Line 1: Format identifier "PARTICLE_SIMULATION_DATA_2D"
- Line 2: "Width <simulation_width>"
- Line 3: "Height <simulation_height>"
- Line 4: "NumBalls <count>"
- Next N lines: Ball definitions (name color radius)
- Next line: "NumSteps <count>"
- For each timestep:
  - "Step <index> <time>"
  - N lines with ball positions: "<ball_index> <x> <y>"

### Supported Colors

Black, Orange, Blue, Red, Green, Purple, Cyan, Brown, Magenta, Yellow

## Animation Controls

- **Play/Pause**: Start or pause animation playback
- **Stop**: Stop and reset to first frame
- **Step Forward/Backward**: Navigate frame-by-frame
- **Timestep Slider**: Jump to specific frame
- **Speed Slider**: Adjust playback speed (1-60 FPS)

## Mouse Controls

- **Left Mouse Drag**: Pan the view
- **Mouse Wheel**: Zoom in/out

## Data Files

Sample particle simulation data files are located in:
```
../../data/ParticleVisualizer2D/
../../data/ParticleVisualizer2D/Basic/
```

Available simulations:
- 2D collision simulations (various grid sizes: 500x500, 1000x800, 1000x1000, 2000x2000)
- Random ball collisions (50, 100, 500, 1000, 2000 balls)
- Brownian motion
- Pressure on walls
- And more...

## Architecture

- **Main Components**:
  - `MainWindow`: Qt GUI with controls and legend
  - `GLWidget`: OpenGL rendering widget with animation
  - `MMLFileParser`: Data file parsing
  - `MMLData`: Data structures (Ball, Vec2D, SimulationData)

- **Rendering**: OpenGL 2D with immediate mode
- **Animation**: QTimer-based frame updates
- **View System**: Orthographic projection with pan/zoom

## Cross-Platform Notes

### Binary Naming
- Windows/macOS: `MML_ParticleVisualizer2D`
- Linux: `MML_ParticleVisualizer2D_Qt` (with suffix)

### OpenGL
- Uses OpenGL 2.1 compatible rendering
- Works on all platforms with standard OpenGL drivers

## License

Part of the MML_Visualizers project.
