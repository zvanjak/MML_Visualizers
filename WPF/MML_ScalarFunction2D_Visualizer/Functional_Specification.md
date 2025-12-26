# Scalar Function 2D Visualizer - Functional Specification

## 1. Overview

The **Scalar Function 2D Visualizer** is a 3D visualization application for displaying scalar functions f(x, y) as surface plots in three-dimensional space. The function maps two input variables (x, y) to a single output value z = f(x, y), which is visualized as a height above the xy-plane.

### 1.1 Purpose
- Display scalar functions f(x, y) as 3D surfaces
- Visualize height (z) as a function of position (x, y)
- Provide interactive 3D camera control
- Enable surface customization (colors, point markers)
- Show data bounds and grid information

### 1.2 Target Use Cases
- Visualizing mathematical surfaces (paraboloids, saddles, peaks)
- Displaying potential fields and energy landscapes
- Analyzing optimization surfaces (loss functions)
- Visualizing probability distributions
- Educational demonstrations of multivariable calculus
- Engineering applications (stress distributions, heat maps)

### 1.3 Key Difference from Parametric Surface Visualizer
| Aspect | Scalar Function 2D | Parametric Surface |
|--------|-------------------|-------------------|
| Definition | z = f(x, y) | x=x(u,v), y=y(u,v), z=z(u,v) |
| Input Variables | (x, y) on regular grid | (u, v) parameters |
| Output | Single z value | 3D point (x, y, z) |
| Surface Type | Always a function graph | Any surface (Möbius strip, torus) |
| Data Storage | 2D matrix of z-values | 3D points grid |
| Typical Use | Height fields, potentials | Arbitrary curved surfaces |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_ScalarFunction2D_Visualizer.exe <file>
```

- **Required**: Exactly one data file path
- Display error message if no file specified
- Does not support multiple files

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1200 pixels |
| Default Height | 800 pixels |
| Title | "Scalar function 2D visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Scalar Function 2D Format (`SCALAR_FUNCTION_CARTESIAN_2D`)

```
SCALAR_FUNCTION_CARTESIAN_2D
<title>
x1: <min_x>
x2: <max_x>
NumPointsX: <count_x>
y1: <min_y>
y2: <max_y>
NumPointsY: <count_y>
<x0> <y0> <z0>
<x1> <y0> <z1>
...
<xN> <yM> <zK>
```

**Example:**
```
SCALAR_FUNCTION_CARTESIAN_2D
Paraboloid: z = x² + y²
x1: -5
x2: 5
NumPointsX: 10
y1: -5
y2: 5
NumPointsY: 10
-5 -5 50
-4.44 -5 44.69
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `SCALAR_FUNCTION_CARTESIAN_2D`) |
| Line 2 | Surface title (displayed at top of window) |
| Line 3 | Minimum X value (x1) |
| Line 4 | Maximum X value (x2) |
| Line 5 | Number of grid points in X direction |
| Line 6 | Minimum Y value (y1) |
| Line 7 | Maximum Y value (y2) |
| Line 8 | Number of grid points in Y direction |
| Lines 9+ | Space-separated (x, y, z) triples |

### 3.2 Data Point Structure

Each data point consists of three values:
| Value | Description |
|-------|-------------|
| x | X coordinate on domain grid |
| y | Y coordinate on domain grid |
| z | Function value: z = f(x, y) |

**Grid Organization:**
- Data arranged in row-major order
- All y values for x?, then all y values for x?, etc.
- Total points = NumPointsX × NumPointsY

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
?                                         ?  ? Display options     ?  ?
?          3D Viewport                    ?  ? ? Show surf points  ?  ?
?       (3D Scene with Surface            ?  ? Surface: [Red   ?]  ?  ?
?        + Coordinate Axes                ?  ? Points:  [Blue  ?]  ?  ?
?        + Optional Point Markers)        ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?     [Interactive 3D Camera Control]     ?  ? Graph bounds        ?  ?
?     • Left Mouse: Rotate                ?  ? X min: -10          ?  ?
?     • Right Mouse: Pan                  ?  ? X max: 10           ?  ?
?     • Mouse Wheel: Zoom                 ?  ? Y min: -10          ?  ?
?     • Arrow Keys: Pan                   ?  ? Y max: 10           ?  ?
?     • +/- Keys: Zoom                    ?  ? Z min: -48          ?  ?
?                                         ?  ? Z max: 48           ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Grid info           ?  ?
?                                         ?  ? Points X: 20        ?  ?
?                                         ?  ? Points Y: 20        ?  ?
?                                         ?  ? Total: 400          ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Large, bold, read-only title |
| 3D Viewport | Left area | Interactive 3D scene with Viewbox scaling |
| Sidebar | Right (220px) | Control panels |

### 4.2 Sidebar Panels

#### 4.2.1 Graph Title Panel
- **Editable TextBox**: Modify the displayed title
- Multi-line support with word wrap
- Changes reflect immediately in the title display

#### 4.2.2 Display Options Panel
- **Show Surface Points**: Checkbox to toggle point markers
  - When checked: Small cubes drawn at each grid point
  - When unchecked: Only surface mesh is visible
- **Surface Color**: Dropdown selector
  - Available: Red, Blue, Green, Orange, Purple, Cyan, Yellow, Gray
  - Default: Red
- **Points Color**: Dropdown selector
  - Available: Blue, Red, Green, Orange, Purple, Cyan, Black, White
  - Default: Blue

#### 4.2.3 Graph Bounds Panel
- **Read-only display** of data bounds:
  - X min, X max: Domain extent in X
  - Y min, Y max: Domain extent in Y
  - Z min, Z max: Range (function values)

#### 4.2.4 Grid Info Panel
- **Read-only display** of grid resolution:
  - Points X: Number of samples in X direction
  - Points Y: Number of samples in Y direction
  - Total: NumPointsX × NumPointsY

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
- Line width: 0.5 units
- Origin at (0, 0, 0)

### 5.2 Coordinate Scaling

| Axis | Scale Factor | Purpose |
|------|--------------|---------|
| X | 10.0 | Map data X to world X |
| Y | 10.0 | Map data Y to world Y |
| Z | 1.0 | Map data Z to world Z (no scaling) |

**Why Scale X and Y?**
- Provides better visual proportions
- Prevents extremely stretched or compressed surfaces
- Keeps surface within reasonable bounds for viewing

### 5.3 Lighting Setup

| Light Type | Color | Configuration |
|------------|-------|---------------|
| Ambient Light | White | Base illumination (from helper) |
| Directional Lights | White | Multiple directional sources (from helper) |

**Note:** Lighting configured via `WorldCameraMouseHelper.InitLights()`

### 5.4 Camera System

**Initial Camera Position:**
- Position: (180, 80, 150)
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

### 5.5 Projection
- **Perspective Camera**: Natural 3D depth perception
- Viewport uses **Viewbox** for automatic scaling
  - Viewbox size: 1600 × 950
  - Stretches to fill available space
  - Maintains aspect ratio

---

## 6. Surface Rendering

### 6.1 Surface Mesh Creation
- Surface constructed from triangular mesh
- Each grid cell (i, j) creates two triangles:
  - Triangle 1: (i,j) ? (i+1,j) ? (i,j+1)
  - Triangle 2: (i+1,j) ? (i+1,j+1) ? (i,j+1)
- Normals computed for proper lighting

### 6.2 Surface Visual Properties

| Property | Description | Configurable |
|----------|-------------|--------------|
| Material | DiffuseMaterial | Yes |
| Color | From dropdown selector | Yes |
| Shading | Smooth (via normals) | No |
| Transparency | Opaque | No |

### 6.3 Point Markers

**When Enabled:**
- Small cubes drawn at each (x, y, z) grid point
- **Cube Size**: 0.75 units
- **Color**: From dropdown selector
- **Material**: DiffuseMaterial
- **Purpose**: Show data sampling points

**Performance Note:**
- Adds N × M geometry models to scene
- For 100×100 grid = 10,000 cubes
- Can impact rendering performance on large grids

---

## 7. Surface Color Palette

### 7.1 Available Surface Colors
| Color | RGB | Typical Use |
|-------|-----|-------------|
| Red | (255, 0, 0) | Default, positive values |
| Blue | (0, 0, 255) | Cool regions |
| Green | (0, 255, 0) | Natural surfaces |
| Orange | (255, 165, 0) | Warm regions |
| Purple | (128, 0, 128) | Special features |
| Cyan | (0, 255, 255) | Water, cool areas |
| Yellow | (255, 255, 0) | Highlights |
| Gray | (128, 128, 128) | Neutral |

### 7.2 Available Points Colors
| Color | RGB | Typical Use |
|-------|-----|-------------|
| Blue | (0, 0, 255) | Default, contrasts with red surface |
| Red | (255, 0, 0) | High-contrast |
| Green | (0, 255, 0) | Nature-themed |
| Orange | (255, 165, 0) | Warm accent |
| Purple | (128, 0, 128) | Distinctive |
| Cyan | (0, 255, 255) | Cool accent |
| Black | (0, 0, 0) | Maximum contrast |
| White | (255, 255, 255) | Against dark surfaces |

---

## 8. Interactive Features

### 8.1 Scene Updates

**Full Redraw (preserves camera):**
- Surface color change
- Points color change
- Show/hide surface points toggle

**Camera Reset:**
- Only on application restart
- No manual "reset camera" button

### 8.2 Display Options

**Show Surface Points Toggle:**
- Immediately adds/removes point markers
- Redraws entire scene
- Camera position preserved

**Color Changes:**
- Changes take effect immediately
- Scene redraws with new colors
- Camera position preserved

### 8.3 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in main title display
- Multi-line titles supported

### 8.4 Camera Persistence
- Camera position/orientation maintained during:
  - Color changes
  - Point marker toggles
  - Window resizing (via Viewbox)
- Only reset on application restart

---

## 9. Error Handling

### 9.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Unsupported format | "Unsupported format: {type}" |
| Parse error | "Error loading file {path}\nMessage: {details}" |
| Point count mismatch | "Count does not match...\nCount = X, Expected = Y" |

### 9.2 Data Edge Cases

| Case | Handling |
|------|----------|
| Empty file | Error message, no visualization |
| Invalid grid size | Error on load |
| NaN/Infinity values | May cause rendering issues |
| Negative grid dimensions | Parse error |

---

## 10. Implementation Architecture

### 10.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `MainWindow` | UI coordination, event handling |
| `WorldCameraMouseHelper` | 3D camera control logic |
| `Geometries` | 3D geometry creation (cubes, surfaces) |
| `Utils` | Scene utilities (coordinate system drawing) |
| `Matrix` (MML) | Stores grid of z-values |

### 10.2 Data Storage

```csharp
class MainWindow
{
    Matrix? _vals;              // Grid of z-values
    double _xMin, _xMax;        // X domain bounds
    double _yMin, _yMax;        // Y domain bounds
    double _zMin, _zMax;        // Z range (computed)
    int _numPointsX, _numPointsY;  // Grid dimensions
    double _scaleX = 10, _scaleY = 10;  // Coordinate scaling
}
```

### 10.3 3D Rendering Pipeline

```
1. Load Data
   ??> Parse file ? Matrix of z-values
   ??> Calculate X, Y, Z bounds
   
2. Initialize Scene
   ??> Setup camera (WorldCameraMouseHelper)
   ??> Setup lighting (via helper)
   ??> Draw coordinate axes
   
3. Draw Surface
   ??> If ShowSurfacePoints:
       ??> For each (i,j):
           ??> Geometries.CreateCube() ? MeshGeometry3D
           ??> Apply points color material
           ??> Add to scene
   
   ??> Geometries.CreateSurface() ? MeshGeometry3D
   ??> Apply surface color material
   ??> Add to scene
   
4. User Interaction
   ??> Color change ? RedrawScene()
   ??> Toggle points ? RedrawScene()
   ??> Camera moves ? Update transform
```

### 10.4 Surface Mesh Generation

```
CreateSurface(Matrix vals, xMin, xMax, yMin, yMax, scaleX, scaleY):
    mesh = new MeshGeometry3D()
    
    // Add vertices
    for i in 0..numX:
        for j in 0..numY:
            x = scaleX * (xMin + i * deltaX)
            y = scaleY * (yMin + j * deltaY)
            z = vals[i,j]
            mesh.Positions.Add(x, y, z)
    
    // Add triangles
    for i in 0..(numX-1):
        for j in 0..(numY-1):
            idx = i * numY + j
            // Triangle 1
            mesh.Indices.Add(idx, idx+numY, idx+1)
            // Triangle 2
            mesh.Indices.Add(idx+numY, idx+numY+1, idx+1)
    
    return mesh
```

---

## 11. Extensibility Points

### 11.1 Color Palettes
- Add more colors to dropdowns
- Implement gradient coloring based on z-value
- Add transparency/opacity controls

### 11.2 Rendering Options
- Add wireframe mode
- Add smooth/flat shading toggle
- Adjust point marker size
- Add contour lines at fixed z-values

### 11.3 Additional Features (Future)
- Color mapping by height (gradient)
- Contour lines projection on xy-plane
- Vertical slices (show f(x, y?) curves)
- Export to image or 3D model format
- Multiple surfaces overlay
- Animation of parameter changes

---

## 12. Platform Porting Notes

### 12.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Data Storage** - Matrix class for z-values
3. **3D Rendering** - Highly platform-specific
4. **Camera Control** - Platform-specific but reusable logic

### 12.2 Platform-Specific Elements
| WPF Element | Qt Equivalent | OpenGL Equivalent |
|-------------|---------------|-------------------|
| Viewport3D | QGLWidget / Qt3D | OpenGL context |
| Viewbox | QGraphicsView with scale | Viewport transform |
| PerspectiveCamera | QCamera | gluLookAt() |
| MeshGeometry3D | QMesh | Vertex/Index arrays |
| DiffuseMaterial | QDiffuseMapMaterial | glMaterial() |
| ComboBox | QComboBox | Custom widget |

### 12.3 Surface Mesh Porting

**WPF (current):**
```csharp
MeshGeometry3D with Positions, TriangleIndices
```

**Qt:**
```cpp
QMesh or custom QGeometry
Vertex buffer + Index buffer
```

**OpenGL:**
```cpp
glVertexAttribPointer() for positions
glDrawElements() for indexed triangles
Manual normal calculation
```

### 12.4 Color Selection Porting

**WPF:** ComboBox with string items, map to Colors

**Qt:** QComboBox with QColor or custom delegate

**OpenGL/FLTK:** Custom color picker widget or menu

---

## Appendix A: Sample Data Files

### A.1 Paraboloid
```
SCALAR_FUNCTION_CARTESIAN_2D
Paraboloid: z = x² + y²
x1: -5
x2: 5
NumPointsX: 5
y1: -5
y2: 5
NumPointsY: 5
-5 -5 50
-5 -2.5 31.25
-5 0 25
-5 2.5 31.25
-5 5 50
-2.5 -5 31.25
-2.5 -2.5 12.5
-2.5 0 6.25
-2.5 2.5 12.5
-2.5 5 31.25
0 -5 25
0 -2.5 6.25
0 0 0
0 2.5 6.25
0 5 25
2.5 -5 31.25
2.5 -2.5 12.5
2.5 0 6.25
2.5 2.5 12.5
2.5 5 31.25
5 -5 50
5 -2.5 31.25
5 0 25
5 2.5 31.25
5 5 50
```

### A.2 Saddle Surface
```
SCALAR_FUNCTION_CARTESIAN_2D
Saddle: z = x² - y²
x1: -3
x2: 3
NumPointsX: 5
y1: -3
y2: 3
NumPointsY: 5
-3 -3 0
-3 -1.5 6.75
-3 0 9
-3 1.5 6.75
-3 3 0
...
```

### A.3 Gaussian Peak
```
SCALAR_FUNCTION_CARTESIAN_2D
Gaussian: z = exp(-(x²+y²))
x1: -3
x2: 3
NumPointsX: 30
y1: -3
y2: 3
NumPointsY: 30
-3 -3 0.000123
...
0 0 1.0
...
```

---

## Appendix B: Common Scalar Functions

| Function | Formula | Description |
|----------|---------|-------------|
| Paraboloid | z = x² + y² | Bowl shape, convex |
| Saddle | z = x² - y² | Hyperbolic paraboloid |
| Gaussian | z = exp(-(x²+y²)/?²) | Bell-shaped peak |
| Cone | z = ?(x²+y²) | Conical surface |
| Ripples | z = sin(?(x²+y²)) | Radial wave pattern |
| Rosenbrock | z = (1-x)² + 100(y-x²)² | Optimization test function |
| Himmelblau | z = (x²+y-11)² + (x+y²-7)² | Multi-modal function |

---

## Appendix C: Performance Considerations

### Grid Size Impact

| Grid Size | Total Points | Cubes (if shown) | Performance |
|-----------|--------------|------------------|-------------|
| 10 × 10 | 100 | 100 | Excellent |
| 20 × 20 | 400 | 400 | Very Good |
| 50 × 50 | 2,500 | 2,500 | Good |
| 100 × 100 | 10,000 | 10,000 | Fair |
| 200 × 200 | 40,000 | 40,000 | Slow |

**Recommendations:**
- For interactive exploration: 20-50 points per dimension
- For high-quality rendering: 100-200 points per dimension
- Disable point markers for grids > 50×50
- Consider LOD (Level of Detail) for very large grids
