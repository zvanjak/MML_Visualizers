# Particle Visualizer 2D - Functional Specification

## 1. Overview

The **Particle Visualizer 2D** is a 2D animation application for playing back pre-computed particle simulation data. It visualizes the time evolution of multiple particles moving within a bounded rectangular container, with support for playback controls, adjustable animation speed, and particle visibility toggles.

### 1.1 Purpose
- Playback pre-recorded particle simulation trajectories
- Visualize multi-particle systems in 2D space
- Support animation playback with start, pause, and reset controls
- Provide adjustable animation speed and refresh rate
- Display particles with configurable colors and sizes

### 1.2 Target Use Cases
- Visualizing collision simulation results (elastic/inelastic collisions)
- Animating molecular dynamics trajectories
- Displaying particle swarm behaviors
- Educational demonstrations of physics simulations
- Gas dynamics and kinetic theory visualizations
- N-body problem solutions

### 1.3 Key Difference from 3D Particle Visualizer
| Aspect | 2D Particle | 3D Particle |
|--------|-------------|-------------|
| Space | 2D plane (x, y) | 3D space (x, y, z) |
| Container | Rectangle | Box/Sphere |
| Camera | Fixed 2D canvas | Interactive 3D camera |
| Rendering | WPF Ellipse shapes | 3D spheres with lighting |
| Scaling | Aspect-ratio preserved | 3D projection |
| Typical Use | Billiards, 2D gas | Molecular dynamics, celestial |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_ParticleVisualizer2D.exe <file>
```

- **Required**: Exactly one data file path
- Display error message if no file specified
- Does not support multiple files

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1300 pixels |
| Default Height | 1055 pixels |
| Title | "Particle visualizer 2D" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Particle Simulation 2D Format (`PARTICLE_SIMULATION_DATA_2D`)

```
PARTICLE_SIMULATION_DATA_2D
Width: <container_width>
Height: <container_height>
NumBalls: <particle_count>
<name_1> <color_1> <radius_1>
<name_2> <color_2> <radius_2>
...
<name_N> <color_N> <radius_N>
NumSteps: <total_steps>
Step 0 <time_0>
<ball_index_0> <x0> <y0>
<ball_index_1> <x1> <y1>
...
Step 1 <time_1>
<ball_index_0> <x0> <y0>
<ball_index_1> <x1> <y1>
...
```

**Example:**
```
PARTICLE_SIMULATION_DATA_2D
Width: 500
Height: 500
NumBalls: 3
Ball_1 Red 10
Ball_2 Blue 15
Ball_3 Green 8
NumSteps: 100
Step 0 0.0
0 100 50
1 200 150
2 300 250
Step 1 0.01
0 102 55
1 198 148
2 305 252
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `PARTICLE_SIMULATION_DATA_2D`) |
| Line 2 | Container width in world units |
| Line 3 | Container height in world units |
| Line 4 | Number of particles |
| Lines 5 to 4+N | Particle attributes: name, color, radius |
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
<ball_0> <x> <y>
<ball_1> <x> <y>
...
```

| Field | Description |
|-------|-------------|
| Step index | Sequential step number (0, 1, 2, ...) |
| Time value | Simulation time at this step |
| Ball index | Particle index (0-based) |
| x, y | Position coordinates in world units |

### 3.4 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported
- Coordinates represent **center** of particle (not top-left)

---

## 4. User Interface Layout

```
???????????????????????????????????????????????????????????????????????
?                     Particle Simulation 2D                           ?
???????????????????????????????????????????????????????????????????????
?                                         ?  ???????????????????????  ?
?  ????????????????????????????????????  ?  ? Simulation title    ?  ?
?  ?                                   ?  ?  ? [Editable TextBox]  ?  ?
?  ?                                   ?  ?  ???????????????????????  ?
?  ?                                   ?  ?  ???????????????????????  ?
?  ?   Canvas (1000x1000 pixels)      ?  ?  ? Simulation controls ?  ?
?  ?                                   ?  ?  ? [Start] [Pause]     ?  ?
?  ?   [Container Border]              ?  ?  ? [Reset]             ?  ?
?  ?                                   ?  ?  ? Current step: 0     ?  ?
?  ?   ? ? ?  Particles (Ellipses)    ?  ?  ? [Progress Bar]      ?  ?
?  ?                                   ?  ?  ???????????????????????  ?
?  ?                                   ?  ?  ???????????????????????  ?
?  ?                                   ?  ?  ? Animation settings  ?  ?
?  ?                                   ?  ?  ? Total steps: 100    ?  ?
?  ?                                   ?  ?  ? Step delay: [10] ms ?  ?
?  ?                                   ?  ?  ? Refresh every: [1]  ?  ?
?  ????????????????????????????????????  ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Particles           ?  ?
?                                         ?  ? ? Ball_1 ?         ?  ?
?                                         ?  ? ? Ball_2 ?         ?  ?
?                                         ?  ? ? Ball_3 ?         ?  ?
?                                         ?  ? ...                 ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Container info      ?  ?
?                                         ?  ? Width: 500          ?  ?
?                                         ?  ? Height: 500         ?  ?
?                                         ?  ? Particles: 3        ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Read-only simulation title |
| Canvas | Left area | 1000×1000 pixel drawing surface |
| Sidebar | Right (220px) | Control panels |

### 4.2 Canvas Details

**Canvas Properties:**
| Property | Value | Purpose |
|----------|-------|---------|
| Width | 1000 pixels | Fixed canvas width |
| Height | 1000 pixels | Fixed canvas height |
| Background | Container rectangle | AntiqueWhite fill |
| Border | Black stroke | Container boundary |

**Coordinate Mapping:**
- World coordinates (from data) ? Screen coordinates (canvas)
- Aspect ratio preserved automatically
- Scaling: Fit largest dimension to 1000 pixels

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
  - Purpose: Performance optimization for large step counts

#### 4.3.4 Particles Panel (Legend)
- Lists all particles with:
  - **Checkbox**: Toggle visibility (currently display-only)
  - **Color circle**: Particle color indicator
  - **Name**: Particle name from file
- Limited to first 10 particles in legend

#### 4.3.5 Container Info Panel
- **Width**: Container width in world units
- **Height**: Container height in world units
- **Particles**: Total number of particles

---

## 5. Coordinate System and Scaling

### 5.1 World to Screen Transformation

**Step 1: Determine scaling factor**
```csharp
if (containerWidth >= containerHeight)
{
    scaleX = 1000.0 / containerWidth;
    scaleY = scaleX;  // Preserve aspect ratio
}
else
{
    scaleY = 1000.0 / containerHeight;
    scaleX = scaleY;  // Preserve aspect ratio
}
```

**Step 2: Apply transformation**
```csharp
screenX = worldX * scaleX;
screenY = worldY * scaleY;
```

**Step 3: Convert particle center to shape top-left**
```csharp
Canvas.SetLeft(ellipse, (worldX - radius) * scaleX);
Canvas.SetTop(ellipse, (worldY - radius) * scaleY);
```

### 5.2 Aspect Ratio Preservation

- Always maintains uniform X/Y scaling
- Fits largest dimension to 1000 pixels
- Container may not fill entire canvas if aspect ratio ? 1:1

**Example:**
| Container Size | Scale | Canvas Usage |
|----------------|-------|--------------|
| 500 × 500 | 2.0 | 1000 × 1000 (full) |
| 1000 × 500 | 1.0 | 1000 × 500 (partial) |
| 400 × 800 | 1.25 | 500 × 1000 (partial) |

---

## 6. Particle Rendering

### 6.1 Visual Representation

**Particle Shape:** WPF `Ellipse`
- Circular shape
- Fill color from particle attributes
- No stroke/border

**Size Calculation:**
```csharp
ellipse.Width = (radius * 2) * scaleX;
ellipse.Height = (radius * 2) * scaleY;
```

### 6.2 Rendering Order

1. Draw container border (background rectangle)
2. Draw all visible particles (ellipses)
3. Particles drawn in order (earlier particles behind later ones)

### 6.3 Particle Colors

Colors specified as **WPF color names** in data file:
| Color Name | RGB | Typical Use |
|------------|-----|-------------|
| Red | (255, 0, 0) | High energy particles |
| Blue | (0, 0, 255) | Low energy particles |
| Green | (0, 255, 0) | Neutral particles |
| Black | (0, 0, 0) | Default |
| Yellow | (255, 255, 0) | Special markers |
| Orange, Purple, Cyan, etc. | Various | Multi-particle systems |

---

## 7. Animation System

### 7.1 Animation State Machine

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

### 7.2 Animation Loop

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
        UpdateDisplay(currentStep);
        UpdateProgressBar(currentStep);
    }
    
    currentStep++;
    sleep(stepDelayMs);
}
```

### 7.3 Animation Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| Step Delay | Time between steps | 10 ms | 1-10000 ms |
| Refresh Every | Display update frequency | 1 | 1-1000 |
| Current Step | Current animation position | 0 | 0 to NumSteps |

**Performance Note:**
- Large step counts (>10,000) benefit from `RefreshEvery > 1`
- Example: 100,000 steps with `RefreshEvery=10` ? 10,000 redraws

### 7.4 Animation Controls

| Control | Action | Available When |
|---------|--------|----------------|
| Start | Begin playback from current step | Idle or Paused |
| Pause | Suspend animation | Running |
| Resume | Continue from paused step | Paused (via Start button) |
| Reset | Jump to step 0, stop animation | Paused or Completed |

---

## 8. Interactive Features

### 8.1 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in top title display
- Multi-line titles supported

### 8.2 Animation Speed Control
- Adjust **Step Delay** before starting or while paused
- Changes take effect on next Start
- Lower values = faster playback

### 8.3 Refresh Rate Control
- Adjust **Refresh Every** to skip display updates
- Improves performance for large simulations
- Example: `RefreshEvery=10` skips 9 out of 10 frames

### 8.4 Progress Tracking
- **Current Step** text display
- **Progress Bar** visual indicator
- Both update based on `RefreshEvery` setting

### 8.5 Particle Visibility
- Legend shows first 10 particles
- Checkboxes currently display-only (no functional toggle)
- **Future enhancement**: Enable/disable individual particles

---

## 9. Error Handling

### 9.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Invalid format | "Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_2D'." |
| Parse error | "Error loading file: {details}" |
| Data loading error | "Error loading data from file {path}" |

### 9.2 Data Validation

| Issue | Handling |
|-------|----------|
| Negative dimensions | Error on load |
| Zero particles | Error on load |
| Missing step data | Exception thrown |
| Invalid color name | WPF default (may crash) |
| Step count mismatch | Uses available steps |

---

## 10. Implementation Architecture

### 10.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `ParticleVisualizer2D_MainWindow` | UI coordination, animation loop |
| `Ball` | Particle data storage (positions, attributes) |
| `LegendWidget` | Particle legend with checkboxes |
| `Ellipse` (WPF) | Visual representation of particles |
| `Canvas` (WPF) | 2D drawing surface |

### 10.2 Data Storage

```csharp
class Ball
{
    string Name;
    string Color;
    double Radius;
    List<Vector2Cartesian> Positions;  // All time steps
    
    Vector2Cartesian Pos(int step);
    void AddPos(Vector2Cartesian pos);
}
```

### 10.3 Rendering Pipeline

```
1. Load Data
   ??> Parse file ? List<Ball>
   ??> Store all positions for all steps
   
2. Initialize Canvas
   ??> Calculate scale factors
   ??> Draw container border
   ??> Create Ellipse for each particle
   
3. Animation Loop (separate thread)
   ??> For each step:
       ??> Sleep(stepDelayMs)
       ??> If (step % refreshEvery == 0):
           ??> Update particle positions
           ??> Update UI (step counter, progress bar)
```

### 10.4 Key Methods

```csharp
bool LoadData(string fileName);
void InitializeVisualization();
void SetBallsPositionToStep(int step);
void Animate();  // Runs on background thread
```

---

## 11. Performance Considerations

### 11.1 Simulation Size Impact

| Particles | Steps | Total Positions | Memory | Render Speed |
|-----------|-------|-----------------|--------|--------------|
| 10 | 100 | 1,000 | ~8 KB | Instant |
| 100 | 1,000 | 100,000 | ~800 KB | Fast |
| 1,000 | 10,000 | 10,000,000 | ~80 MB | Moderate |
| 2,000 | 50,000 | 100,000,000 | ~800 MB | Slow |

### 11.2 Optimization Strategies

**For Large Step Counts:**
- Increase `RefreshEvery` to skip frames
- Example: 50,000 steps with `RefreshEvery=100` ? 500 redraws

**For Many Particles:**
- Limit legend to first 10 (already implemented)
- Consider disabling some particles (future feature)

**For Long Simulations:**
- Increase `StepDelay` to slow playback
- Allow selective range playback (future feature)

---

## 12. Extensibility Points

### 12.1 Enhanced Particle Visibility
- Implement functional checkboxes in legend
- Selectively hide/show particles during playback

### 12.2 Additional Controls
- Jump to specific step (slider or text input)
- Play range selection (start step, end step)
- Reverse playback
- Step-by-step forward/backward buttons

### 12.3 Visual Enhancements
- Particle trails (show last N positions)
- Velocity vectors
- Collision highlighting
- Color by velocity/energy

### 12.4 Export Features
- Export animation to video (MP4, GIF)
- Export specific frames as images
- Export modified simulation data

---

## 13. Platform Porting Notes

### 13.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Data Storage** - `Ball` class with position list
3. **Animation Loop** - Timer/thread-based, platform-specific
4. **2D Rendering** - Platform-specific (Canvas/QPainter/Cairo)

### 13.2 Platform-Specific Elements

| WPF Element | Qt Equivalent | FLTK Equivalent |
|-------------|---------------|-----------------|
| Canvas | QWidget + QPainter | Fl_Widget + fl_draw |
| Ellipse | drawEllipse() | fl_circle() |
| ColorConverter | QColor::fromString() | fl_color() |
| Rectangle | drawRect() | fl_rect() |
| ProgressBar | QProgressBar | Fl_Progress |
| Button | QPushButton | Fl_Button |

### 13.3 Animation Porting

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
PARTICLE_SIMULATION_DATA_2D
Width: 100
Height: 100
NumBalls: 3
Ball_1 Red 5
Ball_2 Blue 5
Ball_3 Green 5
NumSteps: 3
Step 0 0.0
0 10 50
1 50 50
2 90 50
Step 1 0.1
0 15 50
1 50 55
2 85 50
Step 2 0.2
0 20 50
1 50 60
2 80 50
```

### A.2 Collision Simulation
```
PARTICLE_SIMULATION_DATA_2D
Width: 500
Height: 500
NumBalls: 2000
Ball_ Red 2
Ball_ Red 2
... (1998 more red particles)
NumSteps: 50
Step 0 0
0 250 250
1 100 100
... (positions for all 2000 particles)
Step 1 0.1
...
```

---

## Appendix B: Typical Simulation Types

| Simulation Type | Particles | Steps | Container | Duration |
|-----------------|-----------|-------|-----------|----------|
| Elastic Collisions | 5-50 | 1,000-10,000 | 500×500 | 10-100 s |
| Gas Dynamics | 100-2,000 | 10,000-50,000 | 500×500 | 100-500 s |
| Billiards | 15 | 5,000 | 800×400 | 50 s |
| Molecular Dynamics | 500-5,000 | 50,000+ | 1000×1000 | 500+ s |

---

## Appendix C: Animation Settings Guidelines

### C.1 Recommended Settings by Simulation Size

| Step Count | Step Delay | Refresh Every | Effective FPS |
|------------|------------|---------------|---------------|
| <1,000 | 10 ms | 1 | 100 FPS |
| 1,000-10,000 | 10 ms | 1-5 | 100-20 FPS |
| 10,000-100,000 | 5 ms | 10-50 | 20-2 FPS |
| >100,000 | 1 ms | 100+ | 10 FPS |

### C.2 Interactive Playback

For exploration of specific regions:
- Use **Step Delay = 50-100 ms** for slow-motion
- Use **Refresh Every = 1** for smooth motion
- Use **Reset** to restart from beginning
- Use **Pause** to examine specific configurations

### C.3 Performance Testing

For rapid overview:
- Use **Step Delay = 1 ms** for maximum speed
- Use **Refresh Every = 100+** to skip frames
- Monitor memory usage with large simulations
