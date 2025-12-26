# Real Function Visualizer - Functional Specification

## 1. Overview

The **Real Function Visualizer** is a 2D graphing application for visualizing real-valued mathematical functions of the form y = f(x). It supports both single functions and multiple related functions plotted on the same coordinate system.

### 1.1 Purpose
- Display discrete (x, y) data points as continuous curves
- Support comparison of multiple functions on a single graph
- Provide interactive controls for visualization customization
- Enable data exploration through legend-based visibility toggles

### 1.2 Target Use Cases
- Visualizing mathematical function outputs
- Comparing simulation results (e.g., position, velocity, acceleration over time)
- Analyzing numerical computation results
- Educational demonstrations of function behavior

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_RealFunctionVisualizer.exe <file1> [file2] [file3] ...
```

- **Required**: At least one data file path
- **Optional**: Additional files for overlay plotting
- Each file adds its function(s) to the same graph
- Display error message if no file specified

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1200 pixels |
| Default Height | 850 pixels |
| Title | "Real function visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Formats

### 3.1 Single Function Format (`REAL_FUNCTION`)

```
REAL_FUNCTION
<title>
x1: <min_x>
x2: <max_x>
NumPoints: <count>
<x0> <y0>
<x1> <y1>
...
```

**Example:**
```
REAL_FUNCTION
Sine Wave
x1: 0
x2: 6.28
NumPoints: 100
0 0
0.0628 0.0627
0.1256 0.1253
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `REAL_FUNCTION`) |
| Line 2 | Graph title (displayed at top of window) |
| Line 3 | Minimum X value metadata (informational) |
| Line 4 | Maximum X value metadata (informational) |
| Line 5 | Number of data points metadata |
| Lines 6+ | Space-separated (x, y) pairs |

### 3.2 Multi-Function Format (`MULTI_REAL_FUNCTION`)

```
MULTI_REAL_FUNCTION
<title>
<dimension>
<legend_1>
<legend_2>
...
<legend_n>
x1: <min_x>
x2: <max_x>
NumPoints: <count>
<x0> <y0_1> <y0_2> ... <y0_n>
<x1> <y1_1> <y1_2> ... <y1_n>
...
```

**Example:**
```
MULTI_REAL_FUNCTION
Projectile Motion
3
Height (m)
Velocity (m/s)
Acceleration (m/s²)
x1: 0
x2: 10
NumPoints: 100
0 0 50 -9.81
0.1 4.95 49.02 -9.81
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `MULTI_REAL_FUNCTION`) |
| Line 2 | Graph title |
| Line 3 | Number of Y columns (dimension) |
| Lines 4 to 3+n | Legend labels for each function |
| Following lines | Metadata (x1, x2, NumPoints) |
| Data lines | X value followed by n Y values |

### 3.3 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported (e.g., `1.5e-3`, `2.5E+6`)
- Values separated by spaces
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
?           Graph Canvas                  ?  ? ? Function 1 ???   ?  ?
?         (Coordinate System              ?  ? ? Function 2 - -   ?  ?
?          + Function Curves)             ?  ? ? Function 3 ···   ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Graph settings      ?  ?
?                                         ?  ? ? Preserve aspect   ?  ?
?                                         ?  ? X min: -10          ?  ?
?                                         ?  ? X max: 10           ?  ?
?                                         ?  ? Y min: -5           ?  ?
?                                         ?  ? Y max: 15           ?  ?
?                                         ?  ? Num points: 500     ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Large, bold, read-only title |
| Graph Canvas | Left area | Main plotting area |
| Sidebar | Right (230px) | Control panels |

### 4.2 Sidebar Panels

#### 4.2.1 Graph Title Panel
- **Editable TextBox**: Allows user to modify the displayed title
- Multi-line support with word wrap
- Changes reflect immediately in the title display

#### 4.2.2 Legend Panel
- Lists all loaded functions with:
  - **Checkbox**: Toggle function visibility
  - **Color indicator**: Line color swatch
  - **Title**: Function name/legend text
- Interactive: Checking/unchecking redraws the graph

#### 4.2.3 Graph Settings Panel
- **Preserve Aspect Ratio**: Checkbox
  - When checked: X and Y use same scale (1:1 ratio)
  - When unchecked: Independent scaling to fill canvas
- **Bounds Display** (read-only):
  - X min, X max
  - Y min, Y max
  - Number of data points

---

## 5. Coordinate System Rendering

### 5.1 Axis Tick Calculation ("Nice Numbers")

The visualizer automatically calculates visually pleasing axis ticks:

**Algorithm:**
1. Calculate rough tick spacing: `range / (targetTicks - 1)`
2. Find magnitude: `10^floor(log10(spacing))`
3. Normalize to 1-10 range
4. Round to nearest "nice" number: `{1, 2, 2.5, 5, 10}`
5. Round min/max to tick boundaries

**Nice Number Sequence:** 1, 2, 2.5, 5, 10 (and powers of 10)

**Example:**
- Data range: 0.3 to 47.8
- Result: Min=0, Max=50, Ticks at 0, 10, 20, 30, 40, 50

### 5.2 Coordinate System Elements

| Element | Description | Default Style |
|---------|-------------|---------------|
| Grid Lines | Vertical and horizontal at tick positions | Light gray, 0.5px |
| Axes | X-axis at y=0, Y-axis at x=0 | Black, 1.5px |
| Tick Marks | Short perpendicular lines at tick positions | Black, 1px, 5px length |
| Labels | Numeric values at each tick | 11px Segoe UI |

### 5.3 Axis Positioning Rules

- **X-axis**: Drawn at y=0 if 0 is within Y range; otherwise at bottom/top edge
- **Y-axis**: Drawn at x=0 if 0 is within X range; otherwise at left/right edge
- **Labels**: Positioned to avoid overlapping the axes

### 5.4 Label Formatting

| Condition | Format | Example |
|-----------|--------|---------|
| Integer values | No decimals | `10`, `250` |
| Small decimals | Fixed precision | `0.25`, `1.50` |
| Very large (?100000) | Scientific | `1.50E+06` |
| Very small (<0.01) | Scientific | `5.00E-03` |

---

## 6. Function Rendering

### 6.1 Drawing Method
- Functions rendered as **Polyline** (connected line segments)
- Efficient for large data sets (thousands of points)
- Optional point markers (circles at each data point)

### 6.2 Line Styles

| Style | Pattern | Use Case |
|-------|---------|----------|
| Solid | ????????? | Single functions, primary data |
| Dashed | - - - - - | Secondary data, comparisons |
| Dotted | ··········· | Tertiary data |
| DashDot | -·-·-·-·- | Multi-function default |

### 6.3 Color Palette

Functions are automatically assigned colors from this palette (cycles for >10 functions):

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

### 6.4 Draw Style Properties

| Property | Type | Default |
|----------|------|---------|
| Stroke Color | Brush | From palette |
| Stroke Thickness | double | 1.5 px |
| Line Style | enum | Solid (single) / DashDot (multi) |
| Show Points | bool | false |
| Point Radius | double | 3 px |

---

## 7. Coordinate Transformation

### 7.1 World to Screen Conversion

```
screenX = centerX + (worldX * scaleX)
screenY = centerY - (worldY * scaleY)
```

Note: Y is inverted (screen Y increases downward)

### 7.2 Scale Calculation

**Without Aspect Ratio Preservation:**
```
scaleX = (canvasWidth * 0.9) / (xMax - xMin)
scaleY = (canvasHeight * 0.9) / (yMax - yMin)
```

**With Aspect Ratio Preservation:**
```
uniformScale = min(scaleX, scaleY)
scaleX = scaleY = uniformScale
```

### 7.3 Centering
```
centerX = canvasWidth/2 - midPointX * scaleX
centerY = canvasHeight/2 + midPointY * scaleY
```

The 0.9 factor provides a 10% margin around the data.

---

## 8. Interactive Features

### 8.1 Window Resize
- Graph redraws automatically on window resize
- Scales maintain data-to-pixel ratio
- Aspect ratio preserved if checkbox is checked

### 8.2 Legend Interaction
- Click checkbox to toggle function visibility
- Hidden functions excluded from rendering
- Bounds recalculation does NOT occur (preserves original view)

### 8.3 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in main title display
- Multi-line titles supported

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
| Empty Y values | Skip that function |

---

## 10. Implementation Architecture

### 10.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `MainWindow` | UI coordination, event handling |
| `FunctionLoader` | File parsing, factory for function objects |
| `ILoadedFunction` | Interface for all function types |
| `SingleLoadedFunction` | Single y=f(x) function storage and rendering |
| `MultiLoadedFunction` | Multiple related functions with shared X values |
| `CoordSystemRenderer` | Draws axes, grid, ticks, labels |
| `AxisTickCalculator` | Computes nice tick values |
| `CoordTransform` | World?Screen coordinate conversion |
| `FunctionDrawStyle` | Visual styling properties |
| `LegendWidget` | Interactive legend with visibility toggles |

### 10.2 Rendering Pipeline

```
1. Load Data
   ??> FunctionLoader.Load() ? ILoadedFunction
   
2. Calculate Bounds
   ??> Aggregate min/max across all functions
   
3. Initialize Coordinate System
   ??> AxisTickCalculator ? nice bounds
   ??> CoordSystemRenderer.UpdateParams() ? scales
   
4. Draw
   ??> CoordSystemRenderer.Draw() ? grid, axes, ticks
   ??> foreach function: function.Draw() ? polylines
```

---

## 11. Extensibility Points

### 11.1 Adding New File Formats
1. Add new `LoadedType` enum value
2. Implement loader method in `FunctionLoader`
3. Add case to format switch statement

### 11.2 Custom Styling
- Modify `CoordSystemStyle` for axis appearance
- Modify `FunctionDrawStyle` for curve appearance
- Extend `LineBrushes` for more colors

### 11.3 Additional Features (Future)
- Zoom and pan
- Data point tooltips
- Export to image
- Axis labels (X: "Time (s)", Y: "Distance (m)")
- Minor grid lines
- Logarithmic scales

---

## 12. Platform Porting Notes

### 12.1 Core Components to Port
1. **File Loader** - Pure logic, minimal platform dependencies
2. **Axis Calculator** - Pure math, no UI dependencies
3. **Coordinate Transform** - Simple math functions
4. **Rendering** - Platform-specific (Canvas/QPainter/Fl_Widget)

### 12.2 Platform-Specific Elements
| WPF Element | Qt Equivalent | FLTK Equivalent |
|-------------|---------------|-----------------|
| Canvas | QWidget + QPainter | Fl_Widget + fl_draw |
| Polyline | QPainterPath | fl_begin_line/fl_vertex |
| SolidColorBrush | QBrush | Fl_Color |
| TextBlock | drawText() | fl_draw() |
| CheckBox | QCheckBox | Fl_Check_Button |
| Slider | QSlider | Fl_Slider |

### 12.3 Data Structures to Preserve
- `AxisTick` and `AxisTickInfo`
- `FunctionDrawStyle` (adapt brush types)
- `CoordSystemParams` (scale, offset values)
- File format specifications (identical across platforms)

---

## Appendix A: Sample Data Files

### A.1 Single Function
```
REAL_FUNCTION
Damped Oscillation
x1: 0
x2: 10
NumPoints: 5
0 1.0
2.5 0.6065
5.0 0.3679
7.5 0.2231
10.0 0.1353
```

### A.2 Multi-Function
```
MULTI_REAL_FUNCTION
Position vs Velocity
2
Position (m)
Velocity (m/s)
x1: 0
x2: 4
NumPoints: 5
0 0 10
1 9.5 9
2 18 8
3 25.5 7
4 32 6
```
