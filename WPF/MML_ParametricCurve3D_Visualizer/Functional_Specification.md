# Parametric Curve 3D Visualizer - Functional Specification

## 1. Overview

The **Parametric Curve 3D Visualizer** is a 3D visualization application for displaying parametric curves in three-dimensional space defined by three functions x(t), y(t), and z(t), where t is the parameter. It supports interactive 3D camera control and animation to show point movement along curve trajectories.

### 1.1 Purpose
- Display 3D parametric curves r(t) = (x(t), y(t), z(t)) in interactive 3D space
- Support multiple curves overlaid in the same 3D scene
- Provide full 3D camera control (rotate, pan, zoom)
- Enable animation to visualize trajectory motion in space
- Support interactive visibility toggles through legends

### 1.2 Target Use Cases
- Visualizing 3D mathematical curves (helices, knots, space curves)
- Animating particle trajectories in 3D space
- Displaying chaotic attractors (Lorenz, Rössler, etc.)
- Visualizing orbital mechanics and celestial trajectories
- Phase space visualization for 3D dynamical systems
- Educational demonstrations of curve geometry in space

### 1.3 Key Difference from 2D Parametric Curve Visualizer
| Aspect | 2D Parametric | 3D Parametric |
|--------|---------------|---------------|
| Definition | x = x(t), y = y(t) | x = x(t), y = y(t), z = z(t) |
| Data Points | (t, x, y) triples | (t, x, y, z) quadruples |
| Coordinate System | 2D axes with grid | 3D coordinate axes |
| Camera | Fixed 2D view | Interactive 3D camera |
| Animation Markers | 2D circles | 3D spheres |
| Line Rendering | 2D polyline | 3D cylindrical tube |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_ParametricCurve3D_Visualizer.exe <file1> [file2] [file3] ...
```

- **Required**: At least one data file path
- **Optional**: Additional files for overlay plotting
- Each file adds its curve to the same 3D scene
- Display error message if no file specified

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1200 pixels |
| Default Height | 800 pixels |
| Title | "Parametric curve 3D visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 3D Parametric Curve Format (`PARAMETRIC_CURVE_CARTESIAN_3D`)

```
PARAMETRIC_CURVE_CARTESIAN_3D
<title>
t1: <start_parameter>
t2: <end_parameter>
NumPoints: <count>
<t0> <x0> <y0> <z0>
<t1> <x1> <y1> <z1>
<t2> <x2> <y2> <z2>
...
```

**Example:**
```
PARAMETRIC_CURVE_CARTESIAN_3D
Helix
t1: 0
t2: 12.566
NumPoints: 100
0 1 0 0
0.126 0.992 0.126 0.126
0.251 0.969 0.248 0.251
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `PARAMETRIC_CURVE_CARTESIAN_3D`) |
| Line 2 | Curve title (displayed in legend and at top of window) |
| Line 3 | Start parameter value (t1) - metadata |
| Line 4 | End parameter value (t2) - metadata |
| Line 5 | Number of sampled points |
| Lines 6+ | Space-separated (t, x, y, z) quadruples |

### 3.2 Data Point Structure

Each data point consists of four values:
| Value | Description |
|-------|-------------|
| t | Parameter value at this sample |
| x | X coordinate: x(t) |
| y | Y coordinate: y(t) |
| z | Z coordinate: z(t) |

### 3.3 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported (e.g., `1.5e-3`, `2.5E+6`)
- Values separated by spaces (one or more)
- Empty lines in data section are skipped

---

## 4. User Interface Layout

```
???????????????????????????????????????????????????????????????????????
?                           Graph Title                                ?
???????????????????????????????????????????????????????????????????????
?                                         ?  ???????????????????????  ?
?                                         ?  ? Graph title         ?  ?
?                                         ?  ? [Editable TextBox]  ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Legend              ?  ?
?                                         ?  ? ? Curve 1 ?????    ?  ?
?          3D Viewport                    ?  ? ? Curve 2 ?????    ?  ?
?       (3D Scene with Curves             ?  ? ? Curve 3 ?????    ?  ?
?        + Coordinate Axes                ?  ???????????????????????  ?
?        + Animation Spheres)             ?  ???????????????????????  ?
?                                         ?  ? Animation           ?  ?
?     [Interactive 3D Camera Control]     ?  ? [Start Animation]   ?  ?
?     • Left Mouse: Rotate                ?  ? [Pause] [Reset]     ?  ?
?     • Right Mouse: Pan                  ?  ? Speed: [10] pnts/s  ?  ?
?     • Mouse Wheel: Zoom                 ?  ???????????????????????  ?
?     • Arrow Keys: Pan                   ?  ???????????????????????  ?
?     • +/- Keys: Zoom                    ?  ? Display             ?  ?
?                                         ?  ? Line width: [?] [+] ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Curve info          ?  ?
?                                         ?  ? Num points: 100     ?  ?
?                                         ?  ? T min: 0            ?  ?
?                                         ?  ? T max: 12.566       ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Large, bold, read-only title |
| 3D Viewport | Left area | Interactive 3D scene |
| Sidebar | Right (220px) | Control panels |

### 4.2 Sidebar Panels

#### 4.2.1 Graph Title Panel
- **Editable TextBox**: Modify the displayed title
- Multi-line support with word wrap
- Changes reflect immediately in the title display

#### 4.2.2 Legend Panel
- Lists all loaded curves with:
  - **Checkbox**: Toggle curve visibility
  - **Color indicator**: Line color swatch
  - **Title**: Curve name from file
- Interactive: Checking/unchecking redraws the scene
- Hidden curves are excluded from animation

#### 4.2.3 Animation Panel
- **Start Animation**: Begin/resume animation
- **Pause**: Toggle pause/resume state
- **Reset**: Return to initial position (t = t_min)
- **Speed (points/sec)**: Control animation rate

#### 4.2.4 Display Panel
- **Line Width Controls**: 
  - **[?] button**: Decrease line width by 10% (multiply by 0.9)
  - **[+] button**: Increase line width by 10% (multiply by 1.1)

#### 4.2.5 Curve Info Panel
- **Num points**: Total number of sample points (max across all curves)
- **T min**: Start of parameter range (aggregated across all curves)
- **T max**: End of parameter range

---

## 5. 3D Scene Setup

### 5.1 Coordinate System

**Axes Properties:**
| Axis | Color | Length | Position |
|------|-------|--------|----------|
| X-axis | Red | 500 units | Origin to (500, 0, 0) |
| Y-axis | Green | 500 units | Origin to (0, 500, 0) |
| Z-axis | Blue | 500 units | Origin to (0, 0, 500) |

**Visual Style:**
- Axes rendered as cylindrical tubes
- Line width: 1.0 × base line width
- Origin at (0, 0, 0)

### 5.2 Lighting Setup

| Light Type | Color | Direction/Position | Purpose |
|------------|-------|-------------------|---------|
| Ambient Light | White | N/A | Base illumination |
| Directional Light 1 | White | (-0.31, 0.2, -0.61) | Primary lighting |
| Directional Light 2 | White | (0.31, 0.2, -0.61) | Fill lighting |

### 5.3 Camera System

**Initial Camera Position:**
- Position: (350, 100, 350)
- Looking at: Origin (0, 0, 0)
- Up Vector: (0, 1, 0)

**Camera Controls:**
| Action | Input | Behavior |
|--------|-------|----------|
| Rotate | Left Mouse Drag | Orbit around scene center |
| Pan | Right Mouse Drag | Translate camera parallel to view plane |
| Zoom | Mouse Wheel | Move camera closer/farther from center |
| Pan | Arrow Keys | Translate in view plane |
| Zoom | +/- Keys | Incremental zoom in/out |

### 5.4 Projection
- **Perspective Camera**: Natural 3D depth perception
- Field of View adjusts automatically
- Near/Far clipping planes handled by WPF 3D system

---

## 6. Curve Rendering

### 6.1 Drawing Method
- Curves rendered as **3D cylindrical tubes** (not flat polylines)
- Each segment is a true 3D cylinder connecting consecutive points
- Provides proper 3D appearance with lighting and shading
- Cross-section has 10 segments for smoothness

### 6.2 Line Width
| Property | Default | Range | Adjustment |
|----------|---------|-------|------------|
| Base Width | 0.25 units | 0.01 to ~5.0 | ±10% per button click |
| Axis Width | 1.0 × base | Fixed ratio | Scales with line width |

### 6.3 Color Palette
Same 11-color palette as other visualizers:

| Index | Color |
|-------|-------|
| 0 | Black |
| 1 | Blue |
| 2 | Red |
| 3 | Green |
| 4 | Orange |
| 5 | Purple |
| 6 | Brown |
| 7 | Cyan |
| 8 | Magenta |
| 9 | Gray |
| 10 | Yellow |

**Color Assignment:**
- Curves assigned colors sequentially from palette
- Color cycles for >11 curves

---

## 7. Animation System

### 7.1 Animation Markers
**Sphere Properties:**
| Property | First Curve | Other Curves |
|----------|-------------|--------------|
| Shape | Sphere | Sphere |
| Radius | 5 units | 2 units |
| Fill Color | Same as curve | Same as curve |
| Material | DiffuseMaterial | DiffuseMaterial |

**Why Different Sizes?**
- First curve marker is larger (5 units) to distinguish primary trajectory
- Additional curves use smaller markers (2 units) to avoid visual clutter

### 7.2 Animation Controls

| Control | Action |
|---------|--------|
| Start Animation | Begin animation from current position |
| Pause | Freeze at current position; button changes to "Resume" |
| Resume | Continue from paused position |
| Reset | Return spheres to t = t_min position |

### 7.3 Animation Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| Speed | Points per second | 10 |
| Delay | Calculated as 1000/speed ms | 100 ms |
| Marker Radius | Visual size of sphere | 5 (first), 2 (others) |

### 7.4 Animation Behavior
- Spheres move simultaneously on all visible curves
- Hidden curves (via legend) have no spheres
- Animation loops: Returns to start after reaching end
- Speed can be changed while animation is running
- Speed can be changed while paused (takes effect on resume)
- Camera remains fully interactive during animation

---

## 8. Interactive Camera Control

### 8.1 Mouse Controls

| Action | Input | Technical Details |
|--------|-------|-------------------|
| Rotate View | Left Mouse + Drag | Orbital rotation around look-at point |
| Pan View | Right Mouse + Drag | Translate camera in view plane |
| Zoom | Mouse Wheel Scroll | Dolly camera along view direction |

### 8.2 Keyboard Controls

| Action | Keys | Behavior |
|--------|------|----------|
| Pan Left | Left Arrow | Move camera left |
| Pan Right | Right Arrow | Move camera right |
| Pan Up | Up Arrow | Move camera up |
| Pan Down | Down Arrow | Move camera down |
| Zoom In | + or = | Move camera closer |
| Zoom Out | - or _ | Move camera farther |

### 8.3 Camera Persistence
- Camera position/orientation maintained during:
  - Line width adjustments
  - Curve visibility toggles
  - Animation playback
- Only reset on application restart

---

## 9. Interactive Features

### 9.1 Scene Updates
**Full Redraw (preserves camera):**
- Line width adjustment
- Curve visibility toggle
- Animation start/stop

**Camera Reset:**
- Only on application restart
- No manual "reset camera" button

### 9.2 Legend Interaction
- Click checkbox to toggle curve visibility
- Hidden curves excluded from rendering AND animation
- Scene redraws immediately

### 9.3 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in main title display
- Multi-line titles supported

### 9.4 Line Width Adjustment
- Press [+] to increase by 10%
- Press [?] to decrease by 10%
- Affects both curves and coordinate axes
- Takes effect immediately (redraws scene)

---

## 10. Error Handling

### 10.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Empty file | "File is empty: {path}" |
| Unknown format | "Unknown file type: {type}" |
| Parse error | "Error loading file {path}\nMessage: {details}" |

### 10.2 Data Edge Cases

| Case | Handling |
|------|----------|
| < 2 data points | Skip rendering (no curve possible) |
| Invalid speed | Reset to default (10 points/sec) |
| NaN/Infinity values | Skip that point in curve |

---

## 11. Implementation Architecture

### 11.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `MainWindow` | UI coordination, event handling, animation control |
| `LoadedCurve` | Stores (t, x, y, z) data and 3D model reference |
| `Sphere` | Animation marker data and 3D model reference |
| `WorldCameraMouseHelper` | 3D camera control logic |
| `Geometries` | 3D geometry creation (polylines, spheres, cylinders) |
| `Utils` | Scene utilities (coordinate system drawing) |
| `LegendWidget` | Interactive legend with visibility toggles |

### 11.2 Data Storage

```csharp
class LoadedCurve
{
    List<double> _tVals;               // Parameter values
    List<Vector3Cartesian> CurveTrace; // 3D points
    GeometryModel3D? CurveModel;       // 3D scene model
}
```

### 11.3 3D Rendering Pipeline

```
1. Load Data
   ??> Parse file ? LoadedCurve ? List<Vector3Cartesian>
   
2. Initialize Scene
   ??> Setup camera (WorldCameraMouseHelper)
   ??> Setup lighting (Ambient + 2× Directional)
   ??> Draw coordinate axes
   
3. Draw Curves
   ??> For each visible curve:
       ??> Geometries.CreatePolyLine() ? MeshGeometry3D
       ??> Apply DiffuseMaterial with curve color
       ??> Add to Model3DGroup
   
4. Animation (if active)
   ??> For each visible curve:
       ??> Geometries.CreateSphere() ? MeshGeometry3D
       ??> TranslateTransform3D to current position
       ??> Update position each frame
```

### 11.4 Animation State Machine

```
       ???????????????????????????????????????????
       ?                                         ?
       ?                                         ?
   ?????????      Start      ????????????       ?
   ? IDLE  ? ????????????????? RUNNING  ?       ?
   ?????????                 ????????????       ?
       ?                        ?    ?          ?
       ?                  Pause ?    ? End      ?
       ?                        ?    ?          ?
       ?    Reset           ???????????         ?
       ?????????????????????? PAUSED  ?         ?
                            ???????????         ?
                                ?               ?
                                ? Resume        ?
                                ?????????????????
```

---

## 12. Extensibility Points

### 12.1 Adding New Curve Types
1. Extend `LoadedCurve` class
2. Add loader logic to `LoadData()` method
3. Handle new format identifier in file parser

### 12.2 Custom Styling
- Modify line width calculation formulas
- Extend color palette
- Adjust sphere sizes for animation markers

### 12.3 Additional Features (Future)
- Curve length calculation and display
- Tangent/normal/binormal frame visualization
- Curvature and torsion display
- Camera presets (top, side, front views)
- Export animation to video
- Tube radius variation along curve

---

## 13. Platform Porting Notes

### 13.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Data Storage** - List of 3D vectors
3. **Animation Loop** - Timer/thread-based, platform-specific
4. **3D Rendering** - Highly platform-specific

### 13.2 Platform-Specific Elements
| WPF Element | Qt Equivalent | OpenGL Equivalent |
|-------------|---------------|-------------------|
| Viewport3D | QGLWidget / Qt3D | OpenGL context |
| PerspectiveCamera | QCamera | gluLookAt() |
| MeshGeometry3D | QMesh | Vertex arrays |
| GeometryModel3D | Qt3DCore::QEntity | Display list |
| DiffuseMaterial | QDiffuseMapMaterial | glMaterial() |
| TranslateTransform3D | QTransform | glTranslate() |
| DirectionalLight | QDirectionalLight | glLight() |

### 13.3 3D Rendering Considerations

**WPF 3D:**
- Uses Viewport3D with declarative scene graph
- Built-in camera and lighting models
- Automatic depth sorting

**Qt 3D:**
- Use Qt3D framework or QOpenGLWidget
- Similar scene graph approach
- Good WPF equivalent

**Pure OpenGL (FLTK):**
- Manual vertex buffer management
- Explicit camera projection matrices
- Manual depth testing and lighting setup
- More work but full control

### 13.4 Camera Control Porting

**WPF (current):**
```csharp
WorldCameraMouseHelper - wraps PerspectiveCamera
```

**Qt:**
```cpp
QCamera + QFirstPersonCameraController
or custom QMatrix4x4 transforms
```

**OpenGL:**
```cpp
gluLookAt() + manual matrix math
Track mouse deltas, update view matrix
```

---

## Appendix A: Sample Data Files

### A.1 Helix
```
PARAMETRIC_CURVE_CARTESIAN_3D
Helix
t1: 0
t2: 12.566
NumPoints: 5
0 1 0 0
3.1416 -1 0 3.1416
6.2832 1 0 6.2832
9.4248 -1 0 9.4248
12.566 1 0 12.566
```

### A.2 Toroidal Knot
```
PARAMETRIC_CURVE_CARTESIAN_3D
Trefoil Knot (3,2)
t1: 0
t2: 6.283
NumPoints: 5
0 2 0 0
1.571 0 2 -1
3.142 -2 0 0
4.712 0 -2 1
6.283 2 0 0
```

### A.3 Lorenz Attractor
```
PARAMETRIC_CURVE_CARTESIAN_3D
Lorenz attractor
t1: 0
t2: 50
NumPoints: 5000
0 1 1 1
0.01 0.92 1.08 1.27
0.02 0.84 1.15 1.54
...
```

---

## Appendix B: Common 3D Parametric Curves

| Curve | x(t) | y(t) | z(t) | Parameter Range |
|-------|------|------|------|-----------------|
| Helix | cos(t) | sin(t) | t | [0, 4?] |
| Circular Helix | r·cos(t) | r·sin(t) | h·t/(2?) | [0, 4?] |
| Viviani's Curve | (1+cos(t)) | sin(t) | 2·sin(t/2) | [0, 4?] |
| Trefoil Knot | sin(t)+2·sin(2t) | cos(t)?2·cos(2t) | ?sin(3t) | [0, 2?] |
| Toroidal Spiral | (R+r·cos(nt))·cos(t) | (R+r·cos(nt))·sin(t) | r·sin(nt) | [0, 4?] |
| Lissajous 3D | sin(at) | sin(bt) | sin(ct) | [0, 2?] |

---

## Appendix C: 3D Camera Mathematics

### View Matrix
```
ViewMatrix = LookAt(eye, center, up)
  = Translate(-eye) × Rotate(eye-center, up)
```

### Projection Matrix
```
PerspectiveMatrix = Perspective(fov, aspect, near, far)
```

### World to Screen Transform
```
ClipSpace = ProjectionMatrix × ViewMatrix × WorldPosition
ScreenSpace = ClipSpace.xy / ClipSpace.w
```

### Orbital Rotation
```
newEye = center + RotateY(?) × RotateX(?) × (eye - center)
```
