# Particle Visualizer 3D - Functional Specification

## 1. Overview

The **Particle Visualizer 3D** is a 3D animation application for playing back pre-computed particle simulation data in three-dimensional space. It visualizes the time evolution of multiple particles moving within a bounded rectangular container (box), with support for interactive 3D camera control, adjustable animation speed, multiple display modes, and particle visibility toggles.

### 1.1 Purpose
- Playback pre-recorded 3D particle simulation trajectories
- Visualize multi-particle systems in 3D space
- Support animation playback with start, pause, and reset controls
- Provide interactive 3D camera manipulation
- Offer multiple display modes (bounding box, coordinate planes, axes only)
- Enable adjustable animation speed and refresh rate

### 1.2 Target Use Cases
- Visualizing 3D collision simulations (elastic/inelastic collisions)
- Animating molecular dynamics trajectories
- Displaying N-body gravitational simulations
- Visualizing particle swarm behaviors in 3D
- Gas dynamics and kinetic theory in 3D
- Educational demonstrations of 3D physics simulations
- Orbital mechanics and celestial body simulations

### 1.3 Key Difference from 2D Particle Visualizer
| Aspect | 2D Particle | 3D Particle |
|--------|-------------|-------------|
| Space | 2D plane (x, y) | 3D space (x, y, z) |
| Container | Rectangle | Rectangular box (cuboid) |
| Camera | Fixed 2D canvas | Interactive 3D camera |
| Rendering | WPF Ellipse shapes | 3D spheres with lighting |
| Display Modes | Single mode | None / Bounding Box / Coordinate Planes |
| Camera Controls | None | Rotate, pan, zoom, keyboard, reset |
| Visualization | 2D projection | True 3D with perspective |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_ParticleVisualizer3D.exe <file>
```

- **Required**: Exactly one data file path
- Display error message if no file specified
- Does not support multiple files

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1300 pixels |
| Default Height | 1000 pixels |
| Title | "Particle visualizer 3D" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Particle Simulation 3D Format (`PARTICLE_SIMULATION_DATA_3D`)

```
PARTICLE_SIMULATION_DATA_3D
Width: <container_width>
Height: <container_height>
Depth: <container_depth>
NumBalls: <particle_count>
<name_1> <color_1> <radius_1>
<name_2> <color_2> <radius_2>
...
<name_N> <color_N> <radius_N>
NumSteps: <total_steps>
Step 0 <time_0>
<ball_index_0> <x0> <y0> <z0>
<ball_index_1> <x1> <y1> <z1>
...
Step 1 <time_1>
<ball_index_0> <x0> <y0> <z0>
<ball_index_1> <x1> <y1> <z1>
...
```

**Example:**
```
PARTICLE_SIMULATION_DATA_3D
Width: 1000
Height: 1000
Depth: 1000
NumBalls: 3
Ball_1 Red 10
Ball_2 Blue 15
Ball_3 Green 8
NumSteps: 100
Step 0 0.0
0 100 50 200
1 200 150 400
2 300 250 600
Step 1 0.01
0 102 55 198
1 198 148 405
2 305 252 598
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `PARTICLE_SIMULATION_DATA_3D`) |
| Line 2 | Container width in world units |
| Line 3 | Container height in world units |
| Line 4 | Container depth in world units |
| Line 5 | Number of particles |
| Lines 6 to 5+N | Particle attributes: name, color, radius |
| Next line | Total number of simulation steps |
| Step blocks | Each step has header + N position lines |

### 3.2 Particle Attributes

Each particle defined by three attributes:
| Attribute | Description | Format |
|-----------|-------------|--------|
| Name | Particle identifier | String (can contain spaces before color) |
| Color | WPF color name | String (e.g., Red, Blue, Green, Black) |
| Radius | Particle radius | Double (world units) |

**Supported Colors:**
- Standard WPF color names: Red, Blue, Green, Yellow, Black, White, Orange, Purple, Cyan, Magenta, Gray, etc.

### 3.3 Step Data Structure

Each simulation step consists of:
```
Step <step_index> <time_value>
<ball_0> <x> <y> <z>
<ball_1> <x> <y> <z>
...
```

| Field | Description |
|-------|-------------|
| Step index | Sequential step number (0, 1, 2, ...) |
| Time value | Simulation time at this step |
| Ball index | Particle index (0-based) |
| x, y, z | Position coordinates in world units |

### 3.4 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported
- Coordinates represent **center** of particle sphere

---

## 4. User Interface Layout

```
???????????????????????????????????????????????????????????????????????
?                     Particle Simulation 3D                           ?
???????????????????????????????????????????????????????????????????????
?                                         ?  ???????????????????????  ?
?                                         ?  ? Simulation title    ?  ?
?                                         ?  ? [Editable TextBox]  ?  ?
?          3D Viewport                    ?  ???????????????????????  ?
?       (3D Scene with Particles          ?  ???????????????????????  ?
?        + Display Elements               ?  ? Simulation controls ?  ?
?        + Coordinate Axes)               ?  ? [Start] [Pause]     ?  ?
?                                         ?  ? [Reset]             ?  ?
?     [Interactive 3D Camera Control]     ?  ? Current step: 0     ?  ?
?     • Left Mouse: Rotate                ?  ? [Progress Bar]      ?  ?
?     • Right Mouse: Pan                  ?  ???????????????????????  ?
?     • Mouse Wheel: Zoom                 ?  ???????????????????????  ?
?     • Arrow Keys: Pan                   ?  ? Animation settings  ?  ?
?     • +/- Keys: Zoom                    ?  ? Total steps: 100    ?  ?
?                                         ?  ? Step delay: [10] ms ?  ?
?                                         ?  ? Refresh every: [1]  ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Camera              ?  ?
?                                         ?  ? [Look at center]    ?  ?
?                                         ?  ? [Reset camera]      ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Display mode        ?  ?
?                                         ?  ? ? None (axes only)  ?  ?
?                                         ?  ? ? Bounding box      ?  ?
?                                         ?  ? ? Coordinate planes ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Particles           ?  ?
?                                         ?  ? ? Ball_1 ?         ?  ?
?                                         ?  ? ? Ball_2 ?         ?  ?
?                                         ?  ? ? Ball_3 ?         ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Container info      ?  ?
?                                         ?  ? Width: 1000         ?  ?
?                                         ?  ? Height: 1000        ?  ?
?                                         ?  ? Depth: 1000         ?  ?
?                                         ?  ? Particles: 3        ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Read-only simulation title |
| 3D Viewport | Left area | Interactive 3D scene |
| Sidebar | Right (220px) | Control panels |

### 4.2 Viewport Details

**Viewport Properties:**
| Property | Value | Purpose |
|----------|-------|---------|
| Type | WPF Viewport3D | 3D rendering surface |
| Camera | PerspectiveCamera | Natural 3D depth perception |
| Background | White | Clean visualization |
| ClipToBounds | True | Prevent rendering outside |

### 4.3 Sidebar Panels

#### 4.3.1 Simulation Title Panel
- **Editable TextBox**: Modify displayed title
- Height: 40 pixels
- Multi-line support with word wrap
- Changes reflect immediately in top title display

#### 4.3.2 Simulation Controls Panel
- **Start Button**: Begin/resume playback
  - Disabled when running
  - Enabled when paused or completed
- **Pause Button**: Toggle pause/resume
  - Changes to "Resume" when paused
  - Enabled only when running
- **Reset Button**: Return to initial state (step 0)
  - Enabled only when paused or completed
- **Current Step Display**: Shows current step index
- **Progress Bar**: Visual indicator of completion
  - Range: 0 to NumSteps
  - Value: Current step

#### 4.3.3 Animation Settings Panel
- **Total Steps**: Read-only display of NumSteps
- **Step Delay (ms)**: Editable, time between steps
  - Default: 10 ms
  - Valid range: 1 to 10000 ms
- **Refresh Every**: Editable, redraw frequency
  - Default: 1 (redraw every step)
  - Example: 5 = redraw every 5th step
  - Purpose: Performance optimization

#### 4.3.4 Camera Panel
- **Look at Center**: Point camera toward box center
  - Center: (Width/2, Height/2, Depth/2)
- **Reset Camera**: Return to initial position/orientation
  - Position: (1350, 1100, 1350)
  - Looking at: Origin (0, 0, 0)

#### 4.3.5 Display Mode Panel
Three mutually exclusive radio buttons:
- **None (axes only)**: Minimal display
  - Only coordinate axes visible
  - Best for complex simulations
- **Bounding Box**: Default mode
  - 3 semi-transparent planes in positive quadrant
  - XY plane at Z=0 (floor)
  - XZ plane at Y=0 (back wall)
  - YZ plane at X=0 (left wall)
- **Coordinate Planes**: Full coordinate system
  - 3 coordinate planes extending in both directions
  - XY plane (Z=0) - Blue tint
  - XZ plane (Y=0) - Green tint
  - YZ plane (X=0) - Red/Coral tint

#### 4.3.6 Particles Panel (Legend)
- Lists all particles with:
  - **Checkbox**: Toggle visibility (currently display-only)
  - **Color circle**: Particle color indicator
  - **Name**: Particle name from file
- Limited to first 10 particles in legend

#### 4.3.7 Container Info Panel
- **Width**: Container width in world units
- **Height**: Container height in world units
- **Depth**: Container depth in world units
- **Particles**: Total number of particles

---

## 5. 3D Scene Setup

### 5.1 Coordinate System

**Axes Properties:**
| Axis | Color | Length | Position |
|------|-------|--------|----------|
| X-axis | Red | BoxLen × 1.1 | Origin to positive X |
| Y-axis | Green | BoxLen × 1.1 | Origin to positive Y |
| Z-axis | Blue | BoxLen × 1.1 | Origin to positive Z |

**Axis Rendering:**
- Rendered as thin rectangular parallelepipeds
- Thickness: 2.0 units
- Sphere tips at positive ends (radius 8.0)
- In Coordinate Planes mode: extend in both directions

**Constants:**
| Constant | Value | Purpose |
|----------|-------|---------|
| _axisLen | 500 | Default axis length |
| _boxLen | 1000 | Default bounding box size |
| _lineWidth | 0.25 | Default line thickness |

### 5.2 Lighting Setup

| Light Type | Color | Direction | Purpose |
|------------|-------|-----------|---------|
| Directional Light 1 | White | (-0.31, 0.2, -0.61) | Primary lighting |
| Directional Light 2 | White | (0.31, 0.2, -0.61) | Fill lighting |

**Note:** No ambient light - relies on directional lights only

### 5.3 Camera System

**Initial Camera Position:**
- Position: (1350, 1100, 1350)
- Looking at: Origin (0, 0, 0)
- Up Vector: (0, 1, 0)

**Camera Controls:**
| Action | Input | Behavior |
|--------|-------|----------|
| Rotate | Left Mouse Drag | Orbit around look-at point |
| Pan | Right Mouse Drag | Translate camera parallel to view plane |
| Zoom | Mouse Wheel | Move camera closer/farther from center |
| Pan | Arrow Keys | Translate in view plane |
| Zoom | +/- Keys | Incremental zoom in/out |
| Look at Center | Button | Point camera at box center |
| Reset | Button | Return to initial camera state |

### 5.4 Projection
- **Perspective Camera**: Natural 3D depth perception
- Field of View adjusts automatically
- Near/Far clipping planes handled by WPF 3D system

---

## 6. Display Modes

### 6.1 Mode: None (Axes Only)

**Visual Elements:**
- Coordinate axes (X: red, Y: green, Z: blue)
- Axis length: 500 units
- Line width: 0.75 (3× base width)
- Sphere tips at positive ends

**Use Case:**
- Complex simulations with many particles
- Focus on particle motion without distractions
- Performance optimization

### 6.2 Mode: Bounding Box (Default)

**Visual Elements:**
- 3 semi-transparent planes in positive quadrant only
- XY plane at Z=0 (floor) - LightSkyBlue
- XZ plane at Y=0 (back wall) - LightSkyBlue
- YZ plane at X=0 (left wall) - LightSkyBlue
- Opacity: 0.2 (20% transparent)
- Plane thickness: 0.5 units
- Coordinate axes with sphere tips

**Purpose:**
- Define simulation boundaries
- Provide spatial reference
- Show "container walls"

**Plane Properties:**
| Plane | Position | Size | Normal |
|-------|----------|------|--------|
| XY (floor) | (BoxLen/2, BoxLen/2, 0) | BoxLen × BoxLen | (0, 0, 1) |
| XZ (back) | (BoxLen/2, 0, BoxLen/2) | BoxLen × BoxLen | (0, 1, 0) |
| YZ (left) | (0, BoxLen/2, BoxLen/2) | BoxLen × BoxLen | (1, 0, 0) |

### 6.3 Mode: Coordinate Planes

**Visual Elements:**
- 3 coordinate planes extending in BOTH directions from origin
- XY plane (Z=0) - LightBlue tint, opacity 0.12
- XZ plane (Y=0) - LightGreen tint, opacity 0.12
- YZ plane (X=0) - LightCoral tint, opacity 0.12
- Plane size: 2× BoxLen (centered at origin)
- Axes extend in both directions
- Smaller sphere tips at negative ends (radius 4.8)

**Purpose:**
- Mathematical coordinate system visualization
- Symmetrical reference frame
- Educational/scientific presentations

**Color Coding:**
| Plane | Color | Represents |
|-------|-------|------------|
| XY | LightBlue | Horizontal plane |
| XZ | LightGreen | Vertical plane (front-back) |
| YZ | LightCoral | Vertical plane (side) |

---

## 7. Particle Rendering

### 7.1 Visual Representation

**Particle Shape:** 3D Sphere (WPF `MeshGeometry3D`)
- True 3D spherical geometry
- Receives lighting and shading
- Casts shadows (if enabled)

**Material:**
- `DiffuseMaterial` with `SolidColorBrush`
- Color from particle attributes
- Responds to directional lights

### 7.2 Particle Creation

```csharp
MeshGeometry3D sphere = Geometries.CreateSphere(center, radius);
Color ballColor = (Color)ColorConverter.ConvertFromString(color);
var material = new DiffuseMaterial(new SolidColorBrush(ballColor));
GeometryModel3D model = new GeometryModel3D(sphere, material);
```

### 7.3 Particle Animation

**Position Update:**
```csharp
TranslateTransform3D transform = new TranslateTransform3D
{
    OffsetX = position.X,
    OffsetY = position.Y,
    OffsetZ = position.Z
};
particleModel.Transform = transform;
```

### 7.4 Rendering Order

1. Clear scene
2. Setup lighting
3. Add all particle spheres to scene
4. Add display mode elements (planes, axes)
5. Update particle positions

---

## 8. Animation System

### 8.1 Animation State Machine

```
       ???????????????????????????????????????????
       ?                                         ?
       ?                                         ?
   ?????????      Start      ????????????       ?
   ? IDLE  ? ????????????????? RUNNING  ?       ?
   ?????????                 ????????????       ?
       ?                        ?    ?          ?
       ?                  Pause ?    ? Complete ?
       ?                        ?    ?          ?
       ?    Reset           ???????????         ?
       ?????????????????????? PAUSED  ?         ?
                            ???????????         ?
                                ?               ?
                                ? Start         ?
                                ?????????????????
```

### 8.2 Animation Loop

**Pseudo-code:**
```
while (currentStep < numSteps and isRunning)
{
    // Handle pause
    while (isPaused and isRunning)
        sleep(50ms);
    
    // Update display every N steps
    if (currentStep % refreshEvery == 0)
    {
        UpdateParticlePositions(currentStep);
        UpdateProgressBar(currentStep);
    }
    
    currentStep++;
    sleep(stepDelayMs);
}
```

### 8.3 Animation Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| Step Delay | Time between steps | 10 ms | 1-10000 ms |
| Refresh Every | Display update frequency | 1 | 1-1000 |
| Current Step | Current animation position | 0 | 0 to NumSteps |

### 8.4 Animation Controls

| Control | Action | Available When |
|---------|--------|----------------|
| Start | Begin playback from current step | Idle or Paused |
| Pause | Suspend animation | Running |
| Resume | Continue from paused step | Paused (via Start button) |
| Reset | Jump to step 0, stop animation | Paused or Completed |

---

## 9. Interactive Camera Control

### 9.1 Mouse Controls

| Action | Input | Technical Details |
|--------|-------|-------------------|
| Rotate View | Left Mouse + Drag | Orbital rotation around look-at point |
| Pan View | Right Mouse + Drag | Translate camera in view plane |
| Zoom | Mouse Wheel Scroll | Dolly camera along view direction |

### 9.2 Keyboard Controls

| Action | Keys | Behavior |
|--------|------|----------|
| Pan Left | Left Arrow | Move camera left |
| Pan Right | Right Arrow | Move camera right |
| Pan Up | Up Arrow | Move camera up |
| Pan Down | Down Arrow | Move camera down |
| Zoom In | + or = | Move camera closer |
| Zoom Out | - or _ | Move camera farther |

### 9.3 Camera Buttons

**Look at Center:**
- Calculates box center: (Width/2, Height/2, Depth/2)
- Reorients camera to look at this point
- Maintains camera distance

**Reset Camera:**
- Returns to initial position: (1350, 1100, 1350)
- Resets look-at point to origin
- Resets up vector to (0, 1, 0)

### 9.4 Camera Persistence

- Camera maintained during:
  - Animation playback
  - Display mode changes
  - Particle visibility toggles
- Explicit reset only via Reset Camera button

---

## 10. Interactive Features

### 10.1 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in top title display
- Multi-line titles supported

### 10.2 Animation Speed Control
- Adjust **Step Delay** before starting or while paused
- Changes take effect on next Start
- Lower values = faster playback

### 10.3 Refresh Rate Control
- Adjust **Refresh Every** to skip display updates
- Improves performance for large simulations
- Example: `RefreshEvery=10` skips 9 out of 10 frames

### 10.4 Progress Tracking
- **Current Step** text display
- **Progress Bar** visual indicator
- Both update based on `RefreshEvery` setting

### 10.5 Display Mode Switching
- Select via radio buttons
- Scene redraws immediately
- Camera position preserved

### 10.6 Particle Visibility
- Legend shows first 10 particles
- Checkboxes currently display-only
- **Future enhancement**: Enable/disable individual particles

---

## 11. Error Handling

### 11.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Invalid format | "Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_3D'." |
| Parse error | "Error loading file: {details}" |
| Data loading error | "Error loading data from file {path}" |

### 11.2 Data Validation

| Issue | Handling |
|-------|----------|
| Negative dimensions | Error on load |
| Zero particles | Error on load |
| Missing step data | Exception thrown |
| Invalid color name | WPF default (may crash) |
| Step count mismatch | Uses available steps |

---

## 12. Implementation Architecture

### 12.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `ParticleVisualizer3D_MainWindow` | UI coordination, animation loop, scene management |
| `ParticleData3D` | Particle data storage (positions, attributes, 3D model) |
| `WorldCameraMouseHelper` | 3D camera control logic |
| `Geometries` | 3D geometry creation (spheres, boxes, planes) |
| `Utils` | Scene utilities (coordinate system drawing) |
| `LegendWidget` | Particle legend with checkboxes |
| `DisplayMode` (enum) | Display mode enumeration |

### 12.2 Data Storage

```csharp
class ParticleData3D
{
    string Name;
    string Color;
    double Radius;
    List<Vector3Cartesian> Positions;  // All time steps
    GeometryModel3D _geomModel;        // 3D sphere model
    
    Vector3Cartesian Pos(int step);
    void AddPos(Vector3Cartesian pos);
}
```

### 12.3 Rendering Pipeline

```
1. Load Data
   ??> Parse file ? List<ParticleData3D>
   ??> Create sphere geometry for each particle
   ??> Store all positions for all steps
   
2. Initialize Scene
   ??> Setup camera (WorldCameraMouseHelper)
   ??> Setup lighting (2× Directional)
   ??> Add particle spheres to scene
   ??> Draw display mode elements
   
3. Animation Loop (separate thread)
   ??> For each step:
       ??> Sleep(stepDelayMs)
       ??> If (step % refreshEvery == 0):
           ??> Update particle transforms
           ??> Update UI (step counter, progress bar)
```

### 12.4 Key Methods

```csharp
bool LoadData(string fileName);
void InitializeScene();
void RedrawScene();  // Preserves camera
void DrawDisplayElements();  // Based on mode
void DrawBoundingBox();
void DrawCoordinatePlanes();
void SetBallsPositionToStep(int step);
void Animate();  // Runs on background thread
```

---

## 13. Performance Considerations

### 13.1 Simulation Size Impact

| Particles | Steps | 3D Spheres | Total Positions | Memory | Render Speed |
|-----------|-------|------------|-----------------|--------|--------------|
| 10 | 100 | 10 | 1,000 | ~10 KB | Instant |
| 100 | 1,000 | 100 | 100,000 | ~1 MB | Fast |
| 500 | 10,000 | 500 | 5,000,000 | ~40 MB | Moderate |
| 1,000 | 50,000 | 1,000 | 50,000,000 | ~400 MB | Slow |

### 13.2 Optimization Strategies

**For Large Step Counts:**
- Increase `RefreshEvery` to skip frames
- Example: 50,000 steps with `RefreshEvery=100` ? 500 redraws

**For Many Particles:**
- Use "None" display mode (no planes)
- Limit legend to first 10 (already implemented)
- Consider particle visibility toggle (future feature)

**For Complex Scenes:**
- Use "None" or "Bounding Box" mode
- Avoid "Coordinate Planes" (6 large transparent planes)

---

## 14. Extensibility Points

### 14.1 Enhanced Particle Visibility
- Implement functional checkboxes in legend
- Selectively hide/show particles during playback

### 14.2 Additional Display Modes
- Grid lines on planes
- Distance markers
- Particle size scaling option
- Wireframe mode

### 14.3 Additional Camera Features
- Save/restore camera presets
- Predefined views (top, side, front, isometric)
- Camera path animation
- Follow particle mode

### 14.4 Visual Enhancements
- Particle trails (show last N positions)
- Velocity vectors
- Collision highlighting
- Color by velocity/energy
- Particle labels (names/IDs)

### 14.5 Export Features
- Export animation to video (MP4)
- Export specific frames as images
- Export modified simulation data

---

## 15. Platform Porting Notes

### 15.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Data Storage** - `ParticleData3D` class with position list
3. **Animation Loop** - Timer/thread-based, platform-specific
4. **3D Rendering** - Highly platform-specific

### 15.2 Platform-Specific Elements

| WPF Element | Qt Equivalent | OpenGL Equivalent |
|-------------|---------------|-------------------|
| Viewport3D | QGLWidget / Qt3D | OpenGL context |
| PerspectiveCamera | QCamera | gluLookAt() |
| MeshGeometry3D (sphere) | QSphereMesh | gluSphere() |
| GeometryModel3D | Qt3DCore::QEntity | Display list |
| DiffuseMaterial | QDiffuseMapMaterial | glMaterial() |
| TranslateTransform3D | QTransform | glTranslate() |
| DirectionalLight | QDirectionalLight | glLight() |
| ColorConverter | QColor::fromString() | Color lookup |

### 15.3 3D Rendering Porting

**WPF (current):**
```csharp
Viewport3D with declarative scene graph
Built-in lighting and camera
Geometries.CreateSphere()
```

**Qt 3D:**
```cpp
Qt3D framework with QSphereMesh
QCamera + QTransform
Similar scene graph approach
```

**OpenGL (FLTK):**
```cpp
Manual sphere generation (gluSphere or custom mesh)
glLookAt() for camera
glLight() for lighting
glTranslate() for particle movement
```

### 15.4 Animation Porting

**WPF (current):**
```csharp
Task.Run(() => Animate());
Dispatcher.Invoke(() => UpdateUI());
```

**Qt:**
```cpp
QTimer timer;
connect(&timer, &QTimer::timeout, this, &Widget::updateFrame);
timer.start(delayMs);
```

**FLTK:**
```cpp
Fl::add_timeout(delay, timer_callback, this);
```

---

## Appendix A: Sample Data Files

### A.1 Simple 3-Particle System
```
PARTICLE_SIMULATION_DATA_3D
Width: 100
Height: 100
Depth: 100
NumBalls: 3
Ball_1 Red 5
Ball_2 Blue 5
Ball_3 Green 5
NumSteps: 3
Step 0 0.0
0 10 50 20
1 50 50 50
2 90 50 80
Step 1 0.1
0 15 50 25
1 50 55 50
2 85 50 75
Step 2 0.2
0 20 50 30
1 50 60 50
2 80 50 70
```

### A.2 Collision Simulation
```
PARTICLE_SIMULATION_DATA_3D
Width: 1000
Height: 1000
Depth: 1000
NumBalls: 100
Ball_ Red 5
... (99 more particles)
NumSteps: 5000
Step 0 0
0 500 500 500
1 100 100 100
... (positions for all 100 particles)
```

### A.3 Gravitational N-Body
```
PARTICLE_SIMULATION_DATA_3D
Width: 2000
Height: 2000
Depth: 2000
NumBalls: 10
Sun Yellow 50
Mercury Gray 5
Venus Orange 8
Earth Blue 10
... (more planets)
NumSteps: 10000
...
```

---

## Appendix B: Typical Simulation Types

| Simulation Type | Particles | Steps | Container | Duration |
|-----------------|-----------|-------|-----------|----------|
| Elastic Collisions | 5-50 | 1,000-10,000 | 1000³ | 10-100 s |
| Gas Dynamics | 100-1,000 | 10,000-50,000 | 1000³ | 100-500 s |
| N-Body Gravity | 5-20 | 5,000-20,000 | 2000³ | 50-200 s |
| Molecular Dynamics | 500-5,000 | 50,000+ | 1000³ | 500+ s |
| Solar System | 9-10 | 10,000+ | 5000³ | 100+ s |

---

## Appendix C: Display Mode Comparison

| Aspect | None | Bounding Box | Coordinate Planes |
|--------|------|--------------|-------------------|
| Planes | 0 | 3 (one quadrant) | 3 (both directions) |
| Transparency | N/A | 20% | 12% |
| Axis Extension | One direction | One direction | Both directions |
| Sphere Tips | Positive only | Positive only | Positive + negative |
| Visual Complexity | Minimal | Moderate | High |
| Performance | Best | Good | Moderate |
| Use Case | Complex sims | Standard visualization | Mathematical/educational |

---

## Appendix D: Camera Mathematics

### View Matrix
```
ViewMatrix = LookAt(eye, center, up)
  = Translate(-eye) × Rotate(eye-center, up)
```

### Projection Matrix
```
PerspectiveMatrix = Perspective(fov, aspect, near, far)
```

### Transform Order
```
FinalPosition = ProjectionMatrix × ViewMatrix × WorldMatrix × VertexPosition
```

### Camera Control Formulas

**Orbital Rotation:**
```
newEye = center + RotateY(?) × RotateX(?) × (eye - center)
```

**Panning:**
```
newEye = eye + (right × ?x) + (up × ?y)
newCenter = center + (right × ?x) + (up × ?y)
```

**Zooming:**
```
direction = normalize(center - eye)
newEye = eye + direction × zoomFactor
```
