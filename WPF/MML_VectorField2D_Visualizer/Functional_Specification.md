# Vector Field 2D Visualizer - Functional Specification

## 1. Overview

The **Vector Field 2D Visualizer** is a 2D visualization application for displaying vector fields V(x, y) = (V?(x, y), V?(x, y)) as arrow glyphs in the xy-plane. It provides intelligent automatic scaling, magnitude-based color coding, and extensive customization options for vector representation.

### 1.1 Purpose
- Display 2D vector fields as arrow glyphs
- Provide intelligent automatic vector scaling
- Support magnitude-based color coding
- Enable vector normalization for direction-only visualization
- Offer aspect ratio control
- Display field statistics (magnitude min/max/avg)

### 1.2 Target Use Cases
- Visualizing fluid flow fields (velocity fields)
- Displaying gradient fields of scalar functions
- Visualizing electromagnetic fields (E, B fields)
- Showing force fields (gravitational, electrostatic)
- Phase space visualization for 2D dynamical systems
- Educational demonstrations of vector calculus
- Wind maps and weather patterns

### 1.3 Key Difference from 3D Vector Field Visualizer
| Aspect | 2D Vector Field | 3D Vector Field |
|--------|-----------------|-----------------|
| Space | 2D plane (x, y) | 3D space (x, y, z) |
| Vectors | (V?, V?) | (V?, V?, V?) |
| Rendering | 2D arrows on canvas | 3D arrows in viewport |
| Camera | Fixed 2D view | Interactive 3D camera |
| Scaling | Automatic + user slider | Manual adjustment |
| Color Coding | Magnitude gradient | Magnitude gradient or custom |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_VectorField2D_Visualizer.exe <file>
```

- **Required**: Exactly one data file path
- Display error message if no file specified
- Does not support multiple files

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1250 pixels |
| Default Height | 850 pixels |
| Title | "Vector field 2D visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Vector Field 2D Format (`VECTOR_FIELD_2D_CARTESIAN`)

```
VECTOR_FIELD_2D_CARTESIAN
<title>
<x0> <y0> <vx0> <vy0>
<x1> <y1> <vx1> <vy1>
<x2> <y2> <vx2> <vy2>
...
```

**Example:**
```
VECTOR_FIELD_2D_CARTESIAN
rotational_field
-5 -5 5 -5
-5 -3 3 -5
-5 -1 1 -5
-5 1 -1 -5
-5 3 -3 -5
-5 5 -5 -5
-3 -5 5 -3
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `VECTOR_FIELD_2D_CARTESIAN`) |
| Line 2 | Field title (displayed at top of window) |
| Lines 3+ | Space-separated (x, y, vx, vy) quadruples |

### 3.2 Data Point Structure

Each line represents one vector at a specific position:
| Value | Description |
|-------|-------------|
| x | X coordinate of vector base position |
| y | Y coordinate of vector base position |
| vx | X component of vector |
| vy | Y component of vector |

### 3.3 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported (e.g., `1.5e-3`, `2.5E+6`)
- Values separated by spaces (one or more)
- Empty lines are skipped

---

## 4. User Interface Layout

```
???????????????????????????????????????????????????????????????????????
?                        Vector Field 2D                               ?
???????????????????????????????????????????????????????????????????????
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field title         ?  ?
?                                         ?  ? [Editable TextBox]  ?  ?
?          Canvas                         ?  ???????????????????????  ?
?       (Vector arrows with               ?  ???????????????????????  ?
?        color-coded magnitudes)          ?  ? Vector scaling      ?  ?
?                                         ?  ? Magnitude: [1.00]   ?  ?
?     [Arrows centered on data points]    ?  ? [Slider: 0.1-5.0]   ?  ?
?     [Color by magnitude]                ?  ? Arrow size: [8]     ?  ?
?     [Optional axes at origin]           ?  ? [Slider: 2-20]      ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Display options     ?  ?
?                                         ?  ? ? Preserve aspect   ?  ?
?                                         ?  ? ? Normalize vectors ?  ?
?                                         ?  ? ? Color by mag      ?  ?
?                                         ?  ? Arrow: [Black  ?]   ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field bounds        ?  ?
?                                         ?  ? X min: -10          ?  ?
?                                         ?  ? X max: 10           ?  ?
?                                         ?  ? Y min: -10          ?  ?
?                                         ?  ? Y max: 10           ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field info          ?  ?
?                                         ?  ? Num vectors: 400    ?  ?
?                                         ?  ? Max mag: 14.14      ?  ?
?                                         ?  ? Min mag: 0.74       ?  ?
?                                         ?  ? Avg mag: 9.45       ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Read-only field title |
| Canvas | Left area | 2D drawing surface for vectors |
| Sidebar | Right (220px) | Control panels with scrollbar |

### 4.2 Canvas Details

**Canvas Properties:**
| Property | Behavior |
|----------|----------|
| Stretching | Fills available space |
| Margin | 5 pixels on all sides |
| Background | White |
| Resizing | Automatic redraw |

**Coordinate Transformation:**
- World coordinates ? Screen coordinates
- Preserves aspect ratio (optional)
- Automatic margins (10% around data)
- Axes drawn at origin if visible

### 4.3 Sidebar Panels

#### 4.3.1 Field Title Panel
- **Editable TextBox**: Modify displayed title
- Height: 40 pixels
- Multi-line support with word wrap
- Changes reflect immediately in top title display

#### 4.3.2 Vector Scaling Panel
Two independent slider controls:

**Magnitude Scale Slider:**
- Range: 0.1 to 5.0
- Default: 1.0
- Purpose: Multiply automatic scale
- Updates in real-time
- Display: Shows current value (F2 format)

**Arrow Size Slider:**
- Range: 2 to 20
- Default: 8
- Purpose: Arrowhead size in pixels
- Snaps to integer values
- Display: Shows current value (integer)

#### 4.3.3 Display Options Panel
**Checkboxes:**
- **Preserve aspect ratio**: Maintain 1:1 X/Y scaling
  - Default: Unchecked
  - When enabled: Uses minimum of X/Y scale factors
- **Normalize vectors**: Show direction only
  - Default: Unchecked
  - When enabled: All vectors have unit length
- **Color by magnitude**: Gradient coloring
  - Default: Checked
  - When disabled: Uses solid arrow color

**Arrow Color Dropdown:**
- Available: Black, Blue, Red, Green, Orange, Purple
- Default: Black
- Only applies when "Color by magnitude" is disabled

#### 4.3.4 Field Bounds Panel
- **Read-only display** of data bounds:
  - X min, X max: Horizontal extent
  - Y min, Y max: Vertical extent

#### 4.3.5 Field Info Panel
- **Read-only statistics**:
  - Num vectors: Total count
  - Max mag: Largest vector magnitude
  - Min mag: Smallest vector magnitude
  - Avg mag: Average magnitude

---

## 5. Intelligent Automatic Scaling

### 5.1 Optimal Scale Calculation

**Goal:** Make vectors visible without overlapping

**Algorithm:**
```
1. Calculate data bounds (xMin, xMax, yMin, yMax)
2. Estimate grid spacing:
   - estimatedGridSize = sqrt(numVectors)
   - avgSpacingX = (xMax - xMin) / (gridSize - 1)
   - avgSpacingY = (yMax - yMin) / (gridSize - 1)
   - avgSpacing = min(avgSpacingX, avgSpacingY)
3. Calculate optimal scale:
   - optimalScale = (0.7 * avgSpacing) / avgMagnitude
   - Clamp to [0.001, 1000]
```

**Rationale:**
- Vectors should be ~70% of grid spacing
- Prevents overlap while maximizing visibility
- Works well for both uniform and non-uniform grids

### 5.2 Effective Scaling

**Combined Scale:**
```
effectiveScale = optimalScale × magnitudeScale
```

Where:
- `optimalScale`: Calculated automatically
- `magnitudeScale`: User slider value (0.1 to 5.0)

### 5.3 Vector Centering

**Important:** Arrows are **centered** at data points, not starting from them

**Calculation:**
```csharp
halfVx = effectiveScale * vx / 2
halfVy = effectiveScale * vy / 2

startX = pos.x - halfVx
startY = pos.y - halfVy
endX = pos.x + halfVx
endY = pos.y + halfVy
```

**Why Centering?**
- Better visual balance
- Clearer field patterns
- Standard in vector field visualization

---

## 6. Coordinate System

### 6.1 Screen Transform

**Parameters:**
```csharp
_windowWidth, _windowHeight    // Canvas size
_xMin, _xMax, _yMin, _yMax     // Data bounds + margin
_scaleX, _scaleY               // World to screen scale
_centerX, _centerY             // Screen origin offset
```

**World to Screen:**
```
screenX = centerX + worldX * scaleX
screenY = centerY - worldY * scaleY  // Y inverted!
```

**Note:** Y-axis is inverted (screen Y increases downward)

### 6.2 Margin Calculation

- **Margin**: 10% of data range added on all sides
- Example: If data X: [0, 10] ? bounds: [-1, 11]
- Prevents vectors at edges from being cut off

### 6.3 Aspect Ratio Handling

**When Preserve Aspect Ratio = Off (default):**
```csharp
scaleX = availableWidth / xRange
scaleY = availableHeight / yRange
```

**When Preserve Aspect Ratio = On:**
```csharp
uniformScale = min(availableWidth / xRange, availableHeight / yRange)
scaleX = uniformScale
scaleY = uniformScale
```

### 6.4 Axes Drawing

**X-Axis:** Drawn if Y=0 is within bounds
- Horizontal line at screen Y for world Y=0
- Color: LightGray
- Thickness: 1 pixel

**Y-Axis:** Drawn if X=0 is within bounds
- Vertical line at screen X for world X=0
- Color: LightGray
- Thickness: 1 pixel

---

## 7. Arrow Rendering

### 7.1 Arrow Structure

Each arrow consists of:
1. **Main shaft**: Line from start to end
2. **Arrowhead**: Two lines forming a V at the endpoint

### 7.2 Arrow Geometry

**Components:**
```
• Shaft: startPoint ? endPoint
• Arrow angle: 25° (fixed)
• Arrow size: User-controlled slider (2-20 pixels)
```

**Arrowhead Calculation:**
```csharp
screenAngle = atan2(dy, dx)
arrowAngleRad = 25° * ?/180

point1 = end - arrowSize * (cos(angle - arrowAngle), sin(angle - arrowAngle))
point2 = end - arrowSize * (cos(angle + arrowAngle), sin(angle + arrowAngle))
```

### 7.3 Stroke Properties

| Property | Value |
|----------|-------|
| Thickness | 1.5 pixels |
| Cap | Round (default WPF) |
| Join | Miter (default WPF) |

---

## 8. Color Coding

### 8.1 Magnitude-Based Gradient

**When "Color by magnitude" is enabled:**

**Color Scale:** Blue ? Cyan ? Green ? Yellow ? Red

**Mapping:**
```
t = (magnitude - minMag) / (maxMag - minMag)  // Normalize to [0,1]

if (t < 0.25):      Blue ? Cyan     (0,g,255) where g: 0?255
elif (t < 0.5):     Cyan ? Green    (0,255,b) where b: 255?0
elif (t < 0.75):    Green ? Yellow  (r,255,0) where r: 0?255
else:               Yellow ? Red    (255,g,0) where g: 255?0
```

**Visual Interpretation:**
| Color | Magnitude | Typical Meaning |
|-------|-----------|-----------------|
| Blue | Minimum | Weak field |
| Cyan | Low | Below average |
| Green | Medium | Average |
| Yellow | High | Above average |
| Red | Maximum | Strong field |

### 8.2 Solid Color Mode

**When "Color by magnitude" is disabled:**
- All arrows use the selected color from dropdown
- Available: Black, Blue, Red, Green, Orange, Purple
- Default: Black

---

## 9. Interactive Features

### 9.1 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in top title display
- Multi-line titles supported

### 9.2 Magnitude Scaling
- Adjust slider in real-time
- Range: 0.1× to 5.0× of optimal scale
- Useful for fields with large magnitude variations

### 9.3 Arrow Size Control
- Adjust arrowhead size independently
- Smaller arrows: Better for dense fields
- Larger arrows: Better for sparse fields

### 9.4 Vector Normalization
- When enabled: All vectors same length
- Shows **direction** only, ignores magnitude
- Useful for:
  - Direction fields (flow patterns)
  - Eliminating magnitude bias
  - Sparse fields with huge magnitude variations

### 9.5 Aspect Ratio Control
- **Off (default)**: Stretches to fill canvas
- **On**: Maintains 1:1 aspect ratio
- Useful for:
  - Physical accuracy
  - Isotropic fields
  - Comparison with other plots

### 9.6 Window Resizing
- Automatic redraw on size change
- Maintains scaling ratios
- Adjusts margins appropriately

---

## 10. Error Handling

### 10.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Unsupported format | "Unsupported format: {type}" |
| Parse error | "Error loading file: {details}" |

### 10.2 Data Edge Cases

| Case | Handling |
|------|----------|
| Zero vectors | Skipped in rendering |
| Zero range | Adds default range (±1) |
| Single point | Adds ±1 margins |
| All zero magnitudes | Default scale = 1.0 |
| NaN/Infinity | Skipped |

---

## 11. Implementation Architecture

### 11.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `VectorField2D_MainWindow` | UI coordination, rendering |
| `Vector2Repr` | Stores position + vector data |
| `CoordSystemParams` | Coordinate transform parameters |

### 11.2 Data Storage

```csharp
class Vector2Repr
{
    Vector2Cartesian Pos;   // (x, y) position
    Vector2Cartesian Vec;   // (vx, vy) vector
}

class CoordSystemParams
{
    double _windowWidth, _windowHeight;
    double _xMin, _xMax, _yMin, _yMax;
    double _scaleX, _scaleY;
    double _centerX, _centerY;
    int _numPoints;
}
```

### 11.3 Rendering Pipeline

```
1. Load Data
   ??> Parse file ? List<Vector2Repr>
   
2. On Window Loaded
   ??> CalculateDataBounds()
   ??> CalculateOptimalScale()
   ??> InitializeCoordSysParams()
   ??> UpdateUITextBoxes()
   ??> Redraw()
   
3. Redraw Process
   ??> Clear canvas
   ??> InitializeCoordSysParams()  // Update transforms
   ??> DrawCoordSystem()           // Axes if visible
   ??> DrawVectors()               // All arrows
       ??> For each vector:
           ??> Calculate effective scale
           ??> Calculate start/end points (centered)
           ??> Transform to screen coords
           ??> Determine color
           ??> Draw shaft + arrowhead
```

### 11.4 Key Algorithms

**Automatic Scaling:**
```csharp
double xRange = _dataXMax - _dataXMin;
double yRange = _dataYMax - _dataYMin;
int estimatedGridSize = (int)sqrt(_vectors.Count);
double avgSpacingX = xRange / max(1, estimatedGridSize - 1);
double avgSpacingY = yRange / max(1, estimatedGridSize - 1);
double avgSpacing = min(avgSpacingX, avgSpacingY);
_optimalVectorScale = (0.7 * avgSpacing) / _avgMagnitude;
_optimalVectorScale = clamp(_optimalVectorScale, 0.001, 1000);
```

**Vector Centering:**
```csharp
double vx = vector.Vec.X;
double vy = vector.Vec.Y;
if (_normalizeVectors)
{
    double mag = sqrt(vx*vx + vy*vy);
    vx /= mag; vy /= mag;
}
double effectiveScale = _optimalVectorScale * _magnitudeScale;
double halfVx = effectiveScale * vx / 2;
double halfVy = effectiveScale * vy / 2;
startPoint = pos - (halfVx, halfVy)
endPoint = pos + (halfVx, halfVy)
```

---

## 12. Performance Considerations

### 12.1 Vector Count Impact

| Num Vectors | Grid Size | Rendering Speed | Recommendations |
|-------------|-----------|-----------------|-----------------|
| 100 | 10×10 | Instant | Any settings |
| 400 | 20×20 | Very fast | Any settings |
| 1,600 | 40×40 | Fast | Default settings |
| 10,000 | 100×100 | Moderate | Reduce arrow size |
| 40,000 | 200×200 | Slow | Optimize |

### 12.2 Optimization Strategies

**For Large Fields (>10,000 vectors):**
- Reduce arrow size to minimum (2-4)
- Disable "Color by magnitude" (solid color faster)
- Consider subsampling data before visualization

**For Real-Time Updates:**
- Batch redraw operations
- Use double buffering (WPF automatic)
- Minimize slider events

---

## 13. Extensibility Points

### 13.1 Additional Color Schemes
- Implement alternative gradients
- HSV-based coloring
- Discrete color levels (contour-like)
- Custom color maps

### 13.2 Additional Vector Styles
- Streamlines instead of arrows
- Line Integral Convolution (LIC)
- Variable arrow thickness by magnitude
- Arrow length + width by magnitude

### 13.3 Field Analysis Tools
- Display divergence/curl at points
- Critical point detection
- Streamline tracing
- Field line integration

### 13.4 Export Features
- Export to image (PNG, SVG)
- Export vector data
- Export statistics

---

## 14. Platform Porting Notes

### 14.1 Core Components to Port
1. **File Loader** - Pure logic
2. **Data Storage** - Vector2Repr class
3. **Scaling Algorithm** - Pure math
4. **2D Rendering** - Platform-specific

### 14.2 Platform-Specific Elements

| WPF Element | Qt Equivalent | Cairo/FLTK Equivalent |
|-------------|---------------|----------------------|
| Canvas | QWidget + QPainter | cairo_t context |
| Line | drawLine() | cairo_line_to() |
| SolidColorBrush | QPen with QColor | cairo_set_source_rgb() |
| Slider | QSlider | Fl_Slider |
| CheckBox | QCheckBox | Fl_Check_Button |

### 14.3 Rendering Porting

**WPF (current):**
```csharp
Line mainLine = new Line
{
    X1 = x1, Y1 = y1,
    X2 = x2, Y2 = y2,
    Stroke = brush,
    StrokeThickness = 1.5
};
mainCanvas.Children.Add(mainLine);
```

**Qt:**
```cpp
QPainter painter(this);
painter.setPen(QPen(color, 1.5));
painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
```

**Cairo:**
```c
cairo_set_source_rgb(cr, r, g, b);
cairo_set_line_width(cr, 1.5);
cairo_move_to(cr, x1, y1);
cairo_line_to(cr, x2, y2);
cairo_stroke(cr);
```

---

## Appendix A: Sample Data Files

### A.1 Radial Field
```
VECTOR_FIELD_2D_CARTESIAN
radial_field
0 0 0 0
1 0 1 0
0 1 0 1
-1 0 -1 0
0 -1 0 -1
1 1 1 1
-1 1 -1 1
-1 -1 -1 -1
1 -1 1 -1
```

### A.2 Rotational Field
```
VECTOR_FIELD_2D_CARTESIAN
rotational_field
0 0 0 0
1 0 0 1
0 1 -1 0
-1 0 0 -1
0 -1 1 0
```

### A.3 Gradient Field
```
VECTOR_FIELD_2D_CARTESIAN
gradient_of_paraboloid
-2 -2 -4 -4
-2 0 -4 0
-2 2 -4 4
0 -2 0 -4
0 0 0 0
0 2 0 4
2 -2 4 -4
2 0 4 0
2 2 4 4
```

---

## Appendix B: Common Vector Fields

| Field Type | Formula | Description |
|------------|---------|-------------|
| Radial | V = (x, y) | Outward from origin |
| Rotational | V = (-y, x) | Counterclockwise |
| Gradient | V = ?f | Perpendicular to level curves |
| Constant | V = (a, b) | Uniform flow |
| Vortex | V = (-y/(x²+y²), x/(x²+y²)) | Swirling motion |
| Source/Sink | V = (x, y)/(x²+y²) | Radial with 1/r falloff |

---

## Appendix C: Color Gradient Details

### C.1 Gradient Bands

| t Range | Color Transition | RGB Formula |
|---------|------------------|-------------|
| 0.0 - 0.25 | Blue ? Cyan | (0, 255*t/0.25, 255) |
| 0.25 - 0.5 | Cyan ? Green | (0, 255, 255*(1-(t-0.25)/0.25)) |
| 0.5 - 0.75 | Green ? Yellow | (255*(t-0.5)/0.25, 255, 0) |
| 0.75 - 1.0 | Yellow ? Red | (255, 255*(1-(t-0.75)/0.25), 0) |

### C.2 Alternative Gradients (Future)

**Grayscale:**
```
RGB = (255*t, 255*t, 255*t)
```

**Red-White-Blue (Diverging):**
```
if t < 0.5: Red ? White
else: White ? Blue
```

**Viridis (Perceptually Uniform):**
```
[Complex polynomial mapping]
```
