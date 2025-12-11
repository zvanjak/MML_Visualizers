# MML Particle Visualizer 2D (FLTK)

Cross-platform visualizer for 2D particle simulations using FLTK.

## Features

- Load and animate 2D particle simulations
- Playback controls (Play, Pause, Reset)
- Step-by-step navigation with slider
- Color-coded particles with legend
- Time display
- Cross-platform (Windows, Linux, macOS)

## Data Format

The visualizer expects text files with the following format:

```
PARTICLE_SIMULATION_DATA_2D
NumBalls: <number_of_balls>
<Ball_name1> <color1> <radius1>
<Ball_name2> <color2> <radius2>
...
NumSteps: <number_of_steps>
Step <step_num> <time>
<ball_index> <x> <y>
<ball_index> <x> <y>
...
Step <step_num> <time>
...
```

Example:
```
PARTICLE_SIMULATION_DATA_2D
NumBalls: 3
Ball_1 Red 10
Ball_2 Blue 15
Ball_3 Green 12
NumSteps: 2
Step 0 0.0
0 100 50
1 200 150
2 150 100
Step 1 0.1
0 110 55
1 195 148
2 155 105
```

Supported colors: Black, Red, Green, Blue, Yellow, Orange, Purple, Cyan, Magenta, White, Gray, Brown, Pink

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
./MML_ParticleVisualizer2D simulation_data.txt
```

Or use the "Load File" button to load files interactively.

### Controls
- **Play**: Start animation playback
- **Pause**: Pause animation
- **Reset**: Return to first frame
- **Slider**: Manually navigate to specific step
