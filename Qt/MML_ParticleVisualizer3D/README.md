# MML Particle Visualizer 3D

Qt-based 3D particle animation visualizer with OpenGL rendering.

## Features

- **3D Particle Animation**: Visualize particle trajectories over time
- **Interactive Controls**: Start, pause, and restart animations
- **Adjustable Speed**: Control animation playback speed
- **3D Camera**: Orbit, pan, and zoom with mouse
- **Bounding Box**: Optional visualization of simulation bounds
- **Particle Info**: Display particle names, colors, and radii

## Building

### Prerequisites
- Qt 6.10.0 (installed in C:/Dev/Qt/6.10.0/msvc2022_64)
- CMake 3.15+
- Visual Studio 2022

### Build Instructions

```bash
# Run the build script
build.bat

# Or manually:
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

## Usage

```bash
MML_ParticleVisualizer3D.exe <data_file>
```

### Example
```bash
MML_ParticleVisualizer3D.exe ..\..\WPF\MML_ParticleVisualizer3D\data\SimData3D.txt
```

## Data Format

Files must use the `PARTICLE_SIMULATION_DATA_3D` format:

```
PARTICLE_SIMULATION_DATA_3D
NumBalls: 5
Ball_1 Black 10
Ball_2 Red 20
Ball_3 Green 15
Ball_4 Blue 25
Ball_5 Yellow 10
NumSteps: 5
Step 0 0.0
0 100.45 50.298 10
1 23 500 20
...
Step 1 0.1
0 110 55 20
...
```

### Format Details
- Header: `PARTICLE_SIMULATION_DATA_3D`
- `NumBalls: <count>` - Number of particles
- For each ball: `<name> <color> <radius>`
  - Name: String identifier
  - Color: Named color (Red, Green, Blue, etc.)
  - Radius: Particle radius
- `NumSteps: <count>` - Number of timesteps
- For each step:
  - `Step <index> <time>` - Step number and time value
  - For each ball: `<index> <x> <y> <z>` - Position coordinates

## Controls

- **Left Mouse**: Rotate camera around scene
- **Right Mouse**: Pan camera view
- **Mouse Wheel**: Zoom in/out
- **Start Button**: Begin animation playback
- **Pause Button**: Pause animation
- **Restart Button**: Reset to first timestep
- **Delay Spinbox**: Adjust animation speed (milliseconds between steps)
- **Show Bounding Box**: Toggle visualization of simulation bounds

## Sample Data

Test data files are available in the WPF project:
- `WPF/MML_ParticleVisualizer3D/data/SimData3D.txt` - Basic 5-particle simulation
- `WPF/MML_ParticleVisualizer3D/data/collision_sim_3d_example2.txt` - Collision example
- `WPF/MML_ParticleVisualizer3D/data/SimDataGravity.txt` - Gravity simulation

## Implementation Details

- **OpenGL Rendering**: Uses legacy OpenGL with fixed-function pipeline
- **Sphere Rendering**: Parametric sphere generation with 20 slices/stacks
- **Animation**: QTimer-based timestep advancement
- **Camera System**: Spherical coordinates with orbit controls
- **Parser**: Custom parser for PARTICLE_SIMULATION_DATA_3D format

## Architecture

```
MML_ParticleVisualizer3D/
├── main.cpp              - Application entry point
├── MainWindow.cpp/h      - Main window with controls
├── GLWidget.cpp/h        - OpenGL rendering widget
├── MMLFileParser.cpp/h   - Data file parser
├── MMLData.h             - Data structures
├── CMakeLists.txt        - Build configuration
├── build.bat             - Windows build script
└── README.md             - This file
```

## License

Part of the MML_Visualizers project.
