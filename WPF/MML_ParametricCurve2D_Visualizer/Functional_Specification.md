# Parametric Curve 2D Visualizer - Functional Specification

## 1. Overview

The **Parametric Curve 2D Visualizer** is a 2D graphing application for visualizing parametric curves defined by two functions x(t) and y(t), where t is the parameter. It supports animation to show point movement along curve trajectories.

### 1.1 Purpose
- Display parametric curves r(t) = (x(t), y(t)) as continuous paths
- Support multiple curves overlaid on the same coordinate system
- Provide animation to visualize trajectory motion
- Enable accurate geometric representation with aspect ratio preservation
- Support interactive visibility toggles through legends

### 1.2 Target Use Cases
- Visualizing mathematical parametric curves (circles, spirals, Lissajous figures)
- Animating particle trajectories and orbits
- Displaying phase space trajectories (e.g., double pendulum dynamics)
- Comparing multiple related curves (e.g., different initial conditions)
- Educational demonstrations of curve parameterization

### 1.3 Key Difference from Real Function Visualizer
| Aspect | Real Function | Parametric Curve |
|--------|---------------|------------------|
| Definition | y = f(x) | x = x(t), y = y(t) |
| Data Points | (x, y) pairs | (t, x, y) triples |
| Self-intersection | Not possible | Allowed |
| Animation | Not applicable | Points move along trajectory |
| Typical Use | Function plots | Trajectories, orbits, motion paths |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_ParametricCurve2D_Visualizer.exe <file1> [file2] [file3] ...
```

- **Required**: At least one data file path
- **Optional**: Additional files for overlay plotting
- Each file adds its curve to the same graph
- Display error message if no file specified

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1250 pixels |
| Default Height | 850 pixels |
| Title | "Parametric curve 2D visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Parametric Curve Format (`PARAMETRIC_CURVE_CARTESIAN_2D`)

```
PARAMETRIC_CURVE_CARTESIAN_2D
<title>
t1: <start_parameter>
t2: <end_parameter>
NumPoints: <count>
<t0> <x0> <y0>
<t1> <x1> <y1>
<t2> <x2> <y2>
...
```

**Example:**
```
PARAMETRIC_CURVE_CARTESIAN_2D
Unit Circle
t1: 0
t2: 6.283185
NumPoints: 100
0 1 0
0.0628 0.998 0.0628
0.1257 0.992 0.1253
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `PARAMETRIC_CURVE_CARTESIAN_2D`) |
| Line 2 | Curve title (displayed in legend and at top of window) |
| Line 3 | Start parameter value (t1) - metadata |
| Line 4 | End parameter value (t2) - metadata |
| Line 5 | Number of sampled points |
| Lines 6+ | Space-separated (t, x, y) triples |

### 3.2 Data Point Structure

Each data point consists of three values:
| Value | Description |
|-------|-------------|
| t | Parameter value at this sample |
| x | X coordinate: x(t) |
| y | Y coordinate: y(t) |

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
?           Graph Canvas                  ?  ? ? Curve 1 ?????    ?  ?
?         (Coordinate System              ?  ? ? Curve 2 ?????    ?  ?
?          + Parametric Curves            ?  ? ? Curve 3 ?????    ?  ?
?          + Animation Markers)           ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Animation           ?  ?
?                                         ?  ? [Start Animation]   ?  ?
?                                         ?  ? [Pause] [Reset]     ?  ?
?                                         ?  ? Speed: [10] pnts/s  ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Graph settings      ?  ?
?                                         ?  ? ? Preserve aspect   ?  ?
?                                         ?  ? X: -1.5 to 1.5      ?  ?
?                                         ?  ? Y: -1.5 to 1.5      ?  ?
?                                         ?  ? Points: 100         ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Curve info          ?  ?
?                                         ?  ? T min: 0            ?  ?
?                                         ?  ? T max: 6.283        ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Large, bold, read-only title |
| Graph Canvas | Left area | Main plotting area with curves and markers |
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
- Interactive: Checking/unchecking redraws the graph
- Hidden curves are excluded from animation

#### 4.2.3 Animation Panel
- **Start Animation**: Begin/resume animation
- **Pause**: Toggle pause/resume state
- **Reset**: Return to initial position (t = t_min)
- **Speed (points/sec)**: Control animation rate

#### 4.2.4 Graph Settings Panel
- **Preserve Aspect Ratio**: Checkbox
  - **Critical for parametric curves**: Ensures circles appear circular
  - When checked: X and Y use same scale (1:1 ratio)
  - When unchecked: Independent scaling to fill canvas
- **Bounds Display** (read-only):
  - X min, X max
  - Y min, Y max
  - Number of points

#### 4.2.5 Curve Info Panel
- **T min**: Start of parameter range (aggregated across all curves)
- **T max**: End of parameter range

---

## 5. Animation System

### 5.1 Animation Concept
Animation displays a marker (filled circle) at each curve's current position, advancing through the sampled points over time. This visualizes how a point would traverse the curve as the parameter t increases.

### 5.2 Animation Controls

| Control | Action |
|---------|--------|
| Start Animation | Begin animation from current position |
| Pause | Freeze at current position; button changes to "Resume" |
| Resume | Continue from paused position |
| Reset | Return marker to t = t_min position |

### 5.3 Animation Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| Speed | Points per second | 10 |
| Delay | Calculated as 1000/speed ms | 100 ms |
| Marker Radius | Visual size of marker | 6 pixels |

### 5.4 Animation Behavior
- Markers move simultaneously on all visible curves
- Hidden curves (via legend) have no markers
- Animation loops: Returns to start after reaching end
- Speed can be changed while animation is running
- Speed can be changed while paused (takes effect on resume)

### 5.5 Marker Visual Properties

| Property | Value |
|----------|-------|
| Shape | Circle (Ellipse) |
| Size | 12 × 12 pixels (radius = 6) |
| Fill | Same color as corresponding curve |
| Stroke | Black |
| Stroke Thickness | 1 pixel |

---

## 6. Coordinate System

### 6.1 Axis Tick Calculation
Same "nice numbers" algorithm as Real Function Visualizer:
- Preferred values: 1, 2, 2.5, 5, 10 (and powers of 10)
- Automatic bounds rounding for clean labels

### 6.2 Aspect Ratio Preservation
**Particularly important for parametric curves:**
- Circles should appear circular, not elliptical
- Spirals should maintain proper proportions
- Phase space trajectories need accurate geometry

When enabled:
```
uniformScale = min(scaleX, scaleY)
scaleX = scaleY = uniformScale
```

### 6.3 Coordinate System Elements
Same as Real Function Visualizer:
- Grid lines (light gray, 0.5px)
- Axes at x=0 and y=0 (or edges if 0 not in range)
- Tick marks and labels

---

## 7. Curve Rendering

### 7.1 Drawing Method
- Curves rendered as **Polyline** (connected line segments)
- Each (x, y) point connected sequentially by parameter order
- Optional point markers at each sample

### 7.2 Line Style Properties

| Property | Type | Default |
|----------|------|---------|
| Stroke Color | Brush | From palette |
| Stroke Thickness | double | 2.0 px (thicker than functions) |
| Line Style | enum | Solid |
| Show Points | bool | false |
| Point Radius | double | 3 px |

### 7.3 Color Palette
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

---

## 8. Interactive Features

### 8.1 Window Resize
- Graph redraws automatically on window resize
- Animation markers cleared and recreated
- Aspect ratio preserved if checkbox is checked

### 8.2 Legend Interaction
- Click checkbox to toggle curve visibility
- Hidden curves excluded from rendering AND animation
- Bounds recalculation does NOT occur (preserves original view)

### 8.3 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in main title display
- Multi-line titles supported

### 8.4 Speed Adjustment
- Modify speed value while animation is running or paused
- Takes effect immediately (running) or on resume (paused)

---

## 9. Error Handling

### 9.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Empty file | "File is empty." |
| Unknown format | "Unsupported format: {type}" |
| Parse error | "Error loading file {path}\nMessage: {details}" |

### 9.2 Data Edge Cases

| Case | Handling |
|------|----------|
| min = max | Add ±10% padding |
| NaN/Infinity values | Use default range [-10, 10] |
| < 2 data points | Skip rendering (no line possible) |
| Invalid speed | Reset to default (10 points/sec) |

---

## 10. Implementation Architecture

### 10.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `MainWindow` | UI coordination, event handling, animation control |
| `CurveLoader` | File parsing, creates curve objects |
| `ILoadedCurve2D` | Interface for parametric curves |
| `LoadedParamCurve2D` | Stores (t, x, y) data and renders curve |
| `CoordSystemRenderer` | Draws axes, grid, ticks, labels |
| `AxisTickCalculator` | Computes nice tick values |
| `CoordTransform` | World?Screen coordinate conversion |
| `CurveDrawStyle` | Visual styling properties |
| `LegendWidget` | Interactive legend with visibility toggles |

### 10.2 Data Storage

```csharp
class LoadedParamCurve2D
{
    List<double> _tVals;  // Parameter values
    List<double> _xVals;  // X coordinates
    List<double> _yVals;  // Y coordinates
}
```

### 10.3 Animation State Machine

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

### 10.4 Rendering Pipeline

```
1. Load Data
   ??> CurveLoader.Load() ? LoadedParamCurve2D
   
2. Calculate Bounds
   ??> Aggregate min/max X, Y, T across all curves
   
3. Initialize Coordinate System
   ??> AxisTickCalculator ? nice bounds
   ??> CoordSystemRenderer.UpdateParams() ? scales
   
4. Draw
   ??> CoordSystemRenderer.Draw() ? grid, axes, ticks
   ??> foreach curve: curve.Draw() ? polylines
   
5. Animation (if active)
   ??> CreateAnimationMarkers() ? ellipses for each visible curve
   ??> UpdateMarkerPositions(step) ? move markers
```

---

## 11. Extensibility Points

### 11.1 Adding New Curve Types
1. Create new class implementing `ILoadedCurve2D`
2. Add loader method in `CurveLoader`
3. Handle new format identifier in switch statement

### 11.2 Custom Styling
- Modify `CurveDrawStyle` for curve appearance
- Extend color palette in `CurveBrushes`
- Adjust marker properties in `MainWindow`

### 11.3 Additional Features (Future)
- Zoom and pan
- Tangent/normal vector display
- Arc length calculation display
- Velocity vector animation
- Export animation to video/GIF

---

## 12. Platform Porting Notes

### 12.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Axis Calculator** - Pure math, no UI dependencies
3. **Coordinate Transform** - Simple math functions
4. **Animation Loop** - Timer/thread-based, platform-specific
5. **Rendering** - Platform-specific (Canvas/QPainter/Fl_Widget)

### 12.2 Platform-Specific Elements
| WPF Element | Qt Equivalent | FLTK Equivalent |
|-------------|---------------|-----------------|
| Canvas | QWidget + QPainter | Fl_Widget + fl_draw |
| Polyline | QPainterPath | fl_begin_line/fl_vertex |
| Ellipse (marker) | drawEllipse() | fl_circle() |
| DispatcherTimer | QTimer | Fl::add_timeout() |
| Thread.Sleep | QThread::sleep() | usleep() |
| SolidColorBrush | QBrush | Fl_Color |

### 12.3 Animation Porting Considerations
- WPF uses `Task.Run()` + `Dispatcher.Invoke()`
- Qt: Use `QTimer` with slots or `QThread`
- FLTK: Use `Fl::add_timeout()` for periodic updates

### 12.4 Data Structures to Preserve
- `LoadedParamCurve2D` data model (t, x, y lists)
- `CurveDrawStyle` (adapt brush types)
- `CoordSystemParams` (scale, offset values)
- File format specifications (identical across platforms)

---

## Appendix A: Sample Data Files

### A.1 Unit Circle
```
PARAMETRIC_CURVE_CARTESIAN_2D
Unit Circle
t1: 0
t2: 6.283185
NumPoints: 5
0 1 0
1.5708 0 1
3.1416 -1 0
4.7124 0 -1
6.2832 1 0
```

### A.2 Logarithmic Spiral
```
PARAMETRIC_CURVE_CARTESIAN_2D
Log spiral
t1: 0
t2: 10
NumPoints: 5
0 1 0
2.5 0.082 0.005
5 0.00007 -0.00003
7.5 -6e-11 3e-11
10 -8e-14 -5e-14
```

### A.3 Phase Space Trajectory
```
PARAMETRIC_CURVE_CARTESIAN_2D
Double pendulum - phase space
t1: 0
t2: 1
NumPoints: 5
0 0.5 0.101
0.25 -0.15 0.35
0.5 0.12 -0.25
0.75 -0.08 0.18
1 0.05 -0.12
```

---

## Appendix B: Common Parametric Curves

| Curve | x(t) | y(t) | Parameter Range |
|-------|------|------|-----------------|
| Circle | cos(t) | sin(t) | [0, 2?] |
| Ellipse | a·cos(t) | b·sin(t) | [0, 2?] |
| Spiral | t·cos(t) | t·sin(t) | [0, 4?] |
| Lissajous | sin(at) | sin(bt) | [0, 2?] |
| Cycloid | r(t - sin(t)) | r(1 - cos(t)) | [0, 4?] |
| Cardioid | (1-cos(t))cos(t) | (1-cos(t))sin(t) | [0, 2?] |
| Astroid | cos³(t) | sin³(t) | [0, 2?] |
