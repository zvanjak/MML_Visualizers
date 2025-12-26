# Vector Field 3D Visualizer - Functional Specification

## 1. Overview

The **Vector Field 3D Visualizer** is a 3D visualization application for displaying vector fields V(x, y, z) = (V?, V?, V?) as 3D arrow glyphs in three-dimensional space. It provides intelligent automatic scaling, magnitude-based color coding, customizable arrow appearance, and a unique **magnitude cutoff feature** for filtering out high-magnitude vectors.

### 1.1 Purpose
- Display 3D vector fields as 3D arrow glyphs
- Provide intelligent automatic vector scaling
- Support magnitude-based color coding
- Enable **magnitude cutoff** to filter vectors by strength
- Offer vector normalization for direction-only visualization
- Display field statistics (magnitude min/max/avg)
- Provide interactive 3D camera control

### 1.2 Target Use Cases
- Visualizing 3D force fields (gravitational, electromagnetic)
- Displaying fluid flow in 3D (velocity fields)
- Showing gradient fields of 3D scalar functions
- Magnetic/electric field visualization
- Wind patterns in atmosphere
- Computational fluid dynamics (CFD) results
- N-body gravitational fields
- Educational demonstrations of vector calculus

### 1.3 Key Differences from 2D Vector Field Visualizer
| Aspect | 2D Vector Field | 3D Vector Field |
|--------|-----------------|-----------------|
| Space | 2D plane (x, y) | 3D space (x, y, z) |
| Vectors | (V?, V?) | (V?, V?, V?) |
| Data Points | (x, y, vx, vy) | (x, y, z, vx, vy, vz) |
| Arrows | 2D lines with heads | 3D cylindrical arrows |
| Camera | Fixed 2D view | Interactive 3D camera |
| Magnitude Cutoff | No | **Yes - filter by magnitude** |
| Bounding Box | No | Optional display |
| Automatic Scaling | Grid-based (70% spacing) | Grid-based (50% spacing) |

---

## 2. Application Launch

### 2.1 Command Line Interface
```
MML_VectorField3D_Visualizer.exe <file>
```

- **Required**: Exactly one data file path
- Display error message if no file specified
- Does not support multiple files

### 2.2 Window Properties
| Property | Value |
|----------|-------|
| Default Width | 1300 pixels |
| Default Height | 850 pixels |
| Title | "Vector field 3D visualizer" |
| Background | White |
| Resizable | Yes |

---

## 3. Data File Format

### 3.1 Vector Field 3D Format (`VECTOR_FIELD_3D_CARTESIAN`)

```
VECTOR_FIELD_3D_CARTESIAN
<title>
<x0> <y0> <z0> <vx0> <vy0> <vz0>
<x1> <y1> <z1> <vx1> <vy1> <vz1>
<x2> <y2> <z2> <vx2> <vy2> <vz2>
...
```

**Example:**
```
VECTOR_FIELD_3D_CARTESIAN
gravity_force_field
-200 -200 -200 0.000576337 -0.00455405 -0.00455405
-200 -200 -186.207 0.000430734 -0.00512439 -0.00477098
-200 -200 -172.414 0.000257367 -0.00575132 -0.00495804
-200 -200 -158.621 5.54508e-05 -0.00643377 -0.00510265
...
```

**Field Descriptions:**
| Field | Description |
|-------|-------------|
| Line 1 | Format identifier (must be `VECTOR_FIELD_3D_CARTESIAN`) |
| Line 2 | Field title (displayed at top of window) |
| Lines 3+ | Space-separated (x, y, z, vx, vy, vz) sextuples |

### 3.2 Data Point Structure

Each line represents one vector at a specific 3D position:
| Value | Description |
|-------|-------------|
| x | X coordinate of vector base position |
| y | Y coordinate of vector base position |
| z | Z coordinate of vector base position |
| vx | X component of vector |
| vy | Y component of vector |
| vz | Z component of vector |

### 3.3 Numeric Format
- All numeric values use **invariant culture** (decimal point `.`)
- Scientific notation supported (e.g., `5.54508e-05`, `1.5E-3`)
- Values separated by spaces (one or more)
- Empty lines are skipped

---

## 4. User Interface Layout

```
???????????????????????????????????????????????????????????????????????
?                        Vector Field 3D                               ?
???????????????????????????????????????????????????????????????????????
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field title         ?  ?
?                                         ?  ? [Editable TextBox]  ?  ?
?          3D Viewport                    ?  ???????????????????????  ?
?       (3D arrows with                   ?  ???????????????????????  ?
?        color-coded magnitudes)          ?  ? Vector scaling      ?  ?
?                                         ?  ? Magnitude: [1.00]   ?  ?
?     [Interactive 3D Camera Control]     ?  ? [Slider: 0.1-5.0]   ?  ?
?     • Left Mouse: Rotate                ?  ? Arrow width: [1.00] ?  ?
?     • Right Mouse: Pan                  ?  ? [Slider: 0.2-3.0]   ?  ?
?     • Mouse Wheel: Zoom                 ?  ???????????????????????  ?
?     • Arrow Keys: Pan                   ?  ???????????????????????  ?
?     • +/- Keys: Zoom                    ?  ? Magnitude cutoff ?  ?  ?
?                                         ?  ? Max: [43.25]        ?  ?
?                                         ?  ? [Slider: 0-100%]    ?  ?
?                                         ?  ? Cutoff: [100%]      ?  ?
?                                         ?  ? Showing: 8000/8000  ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Display options     ?  ?
?                                         ?  ? ? Color by mag      ?  ?
?                                         ?  ? ? Normalize vectors ?  ?
?                                         ?  ? ? Show bounding box ?  ?
?                                         ?  ? Arrow: [Blue   ?]   ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Camera controls     ?  ?
?                                         ?  ? [Look at center]    ?  ?
?                                         ?  ? [Reset camera]      ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field bounds        ?  ?
?                                         ?  ? X: -200 to 200      ?  ?
?                                         ?  ? Y: -200 to 200      ?  ?
?                                         ?  ? Z: -200 to 200      ?  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ???????????????????????  ?
?                                         ?  ? Field info          ?  ?
?                                         ?  ? Num vectors: 8000   ?  ?
?                                         ?  ? Max mag: 0.087496   ?  ?
?                                         ?  ? Min mag: 0.000576   ?  ?
?                                         ?  ? Avg mag: 0.043245   ?  ?
?                                         ?  ???????????????????????  ?
???????????????????????????????????????????????????????????????????????
```

### 4.1 Main Components

| Component | Location | Description |
|-----------|----------|-------------|
| Title Display | Top center | Read-only field title |
| 3D Viewport | Left area | Interactive 3D scene with arrows |
| Sidebar | Right (230px) | Control panels with scrollbar |

### 4.2 Viewport Details

**Viewport Properties:**
| Property | Value | Purpose |
|----------|-------|---------|
| Type | WPF Viewport3D | 3D rendering surface |
| Camera | PerspectiveCamera | Natural 3D depth perception |
| Background | White | Clean visualization |

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

**Arrow Width Slider:**
- Range: 0.2 to 3.0
- Default: 1.0
- Purpose: Adjust arrow thickness
- Updates in real-time
- Display: Shows current value (F2 format)

#### 4.3.3 Magnitude Cutoff Panel ? **UNIQUE FEATURE**
This is a **key distinguishing feature** of the 3D visualizer!

**Max Magnitude TextBox:**
- Editable cutoff value
- Default: Maximum field magnitude
- Press Enter to apply
- Clamped to [minMag, maxMag]
- Format: Scientific notation if needed

**Cutoff Percentage Slider:**
- Range: 0% to 100%
- Default: 100%
- Maps linearly to [minMag, maxMag]
- Updates cutoff value in real-time
- Synced bidirectionally with textbox

**Vectors Shown Display:**
- Format: "Showing: X / Y"
- X = Visible vectors (magnitude ? cutoff)
- Y = Total vectors in field
- Updates dynamically

**Purpose:**
- Filter out high-magnitude vectors that dominate visualization
- Focus on weaker field regions
- Reduce visual clutter in dense fields
- Interactive exploration of magnitude structure

#### 4.3.4 Display Options Panel
**Checkboxes:**
- **Color by magnitude**: Gradient coloring
  - Default: Checked
  - When disabled: Uses solid arrow color
- **Normalize vectors**: Show direction only
  - Default: Unchecked
  - When enabled: All arrows same length
- **Show bounding box**: Display field bounds
  - Default: Unchecked
  - Shows wire-frame box around data region

**Arrow Color Dropdown:**
- Available: Blue, Red, Green, Orange, Purple, Cyan
- Default: Blue
- Only applies when "Color by magnitude" is disabled

#### 4.3.5 Camera Controls Panel
- **Look at Center**: Point camera toward field center
  - Center: ((xMin+xMax)/2, (yMin+yMax)/2, (zMin+zMax)/2)
- **Reset Camera**: Return to initial position
  - Position: Center + (0.6, 0.4, 0.6) × distance
  - Distance: 2× max range

#### 4.3.6 Field Bounds Panel
- **Read-only display** of data bounds:
  - X range: xMin to xMax
  - Y range: yMin to yMax
  - Z range: zMin to zMax

#### 4.3.7 Field Info Panel
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
1. Calculate data bounds (xMin, xMax, yMin, yMax, zMin, zMax)
2. Estimate grid spacing:
   - estimatedGridSize = cubeRoot(numVectors)
   - avgSpacingX = (xMax - xMin) / (gridSize - 1)
   - avgSpacingY = (yMax - yMin) / (gridSize - 1)
   - avgSpacingZ = (zMax - zMin) / (gridSize - 1)
   - avgSpacing = min(avgSpacingX, avgSpacingY, avgSpacingZ)
3. Calculate optimal scale:
   - optimalScale = (0.5 * avgSpacing) / avgMagnitude
   - Clamp to [0.001, 100000]
```

**Rationale:**
- Vectors should be ~50% of grid spacing (less than 2D's 70%)
- Prevents overlap in 3D space (more critical than 2D)
- Works well for both uniform and non-uniform grids

### 5.2 Effective Scaling

**Combined Scale:**
```
effectiveScale = optimalScale × magnitudeScale
```

Where:
- `optimalScale`: Calculated automatically (cube root based)
- `magnitudeScale`: User slider value (0.1 to 5.0)

### 5.3 **Linear Scaling** (NOT logarithmic)

**Important:** Arrow length is **linearly proportional** to magnitude

```csharp
if (normalizeVectors)
{
    // All arrows same length (use avg magnitude as reference)
    length = effectiveScale * avgMagnitude;
}
else
{
    // Linear scaling - length proportional to magnitude
    length = effectiveScale * magnitude;
}
```

**Arrow Dimensions (all proportional to length):**
- Base width: length × 0.08 × arrowWidthScale
- Head width: length × 0.18 × arrowWidthScale
- Head length: length × 0.25
- Minimum base width: 0.1 units
- Minimum head width: 2× base width

---

## 6. 3D Coordinate System

### 6.1 Coordinate Axes

**Axes Properties:**
| Axis | Color | Length | Thickness | Tip Radius |
|------|-------|--------|-----------|------------|
| X-axis | Red | 0.6 × maxRange | 0.5 units | 8 units |
| Y-axis | Green | 0.6 × maxRange | 0.5 units | 8 units |
| Z-axis | Blue | 0.6 × maxRange | 0.5 units | 8 units |

**Visual Style:**
- Axes rendered as cylindrical rods
- Sphere tips at positive ends
- Origin at (0, 0, 0)
- Always visible

### 6.2 Bounding Box (Optional)

**When "Show bounding box" is checked:**
- 12 edges forming rectangular box
- Wire-frame style using thin cylinders
- Color: Gray
- Thickness: 0.002 × maxRange
- 6 segments per cylinder (hexagonal cross-section)

**Box Edges:**
- 4 bottom edges (at zMin)
- 4 top edges (at zMax)
- 4 vertical edges

### 6.3 Lighting Setup

| Light Type | Color | Direction | Purpose |
|------------|-------|-----------|---------|
| Ambient Light | White | N/A | Base illumination |
| Directional Light 1 | White | (-0.31, 0.2, -0.61) | Primary lighting |
| Directional Light 2 | White | (0.31, 0.2, -0.61) | Fill lighting |

### 6.4 Initial Camera Setup

**Camera Position Calculation:**
```csharp
double xRange = xMax - xMin;
double yRange = yMax - yMin;
double zRange = zMax - zMin;
double maxRange = max(xRange, yRange, zRange);
double distance = maxRange * 2.0;

Point3D centerPoint = ((xMin+xMax)/2, (yMin+yMax)/2, (zMin+zMax)/2);
Point3D cameraPos = centerPoint + (distance×0.6, distance×0.4, distance×0.6);
```

**Initial View Direction:**
```csharp
LookDirection = centerPoint - cameraPos;
UpDirection = (0, 1, 0);
```

---

## 7. 3D Arrow Rendering

### 7.1 Arrow Structure

Each 3D arrow consists of:
1. **Cylindrical shaft**: Base of arrow
2. **Conical head**: Arrow tip (wider cylinder tapering)
3. **8 segments**: Cross-section smoothness

### 7.2 Arrow Geometry Creation

**Arrow is generated by `Geometries.CreateVectorArrow()`:**
```csharp
MeshGeometry3D arrow = Geometries.CreateVectorArrow(
    baseWidth,    // Shaft radius
    length,       // Total arrow length
    8,            // Shaft segments (octagonal)
    5,            // Head segments (conical resolution)
    headWidth,    // Head base radius
    headLength    // Cone height
);
```

### 7.3 Arrow Positioning and Orientation

**Translation:**
- Arrow starts at vector position (x, y, z)
- No centering - arrow **originates from data point**

**Rotation:**
- Default arrow direction: +Z axis (0, 0, 1)
- Rotated to align with vector direction (vx, vy, vz)
- Uses axis-angle rotation via cross product
- Special handling for parallel/antiparallel cases

**Rotation Algorithm:**
```csharp
Vector3D currentDir = (0, 0, 1);  // Arrow default direction
Vector3D newDir = normalize(vx, vy, vz);  // Desired direction

Vector3D axis = crossProduct(currentDir, newDir);
double angle = vectorAngle(currentDir, newDir) * (180/?);

if (axis.length < 1e-10)  // Parallel or antiparallel
{
    if (angle < 90°)
        // Already aligned
    else
        // Rotate 180° around X-axis
}
else
{
    // Rotate by angle around axis
}
```

---

## 8. Magnitude Cutoff Feature ?

### 8.1 Concept

**Purpose:** Filter vectors by magnitude to focus on specific strength ranges

**How It Works:**
- Only vectors with `magnitude ? cutoffValue` are rendered
- Vectors above cutoff are **completely excluded** from scene
- Reduces visual clutter and improves performance
- Enables exploration of field structure at different scales

### 8.2 Cutoff Mechanisms

**Two Input Methods (bidirectionally synced):**

**1. Slider (Percentage):**
- Range: 0% to 100%
- Maps linearly: `cutoff = minMag + (maxMag - minMag) × (percent/100)`
- Real-time updates

**2. TextBox (Absolute Value):**
- Enter desired cutoff magnitude
- Press Enter to apply
- Clamped to [minMag, maxMag]
- Updates slider position

### 8.3 Color Coding with Cutoff

**Important:** Color scale adapts to cutoff!

```
t = (magnitude - minMag) / (cutoff - minMag)  // NOT (maxMag - minMag)!
```

**Why:** Colors represent strength **relative to visible range**

**Example:**
- Field: magnitude ? [0.001, 1.000]
- Cutoff: 0.100 (10%)
- Visible: magnitude ? [0.001, 0.100]
- Blue: ~0.001, Red: ~0.100

### 8.4 Use Cases

| Cutoff % | Purpose | Typical Scenario |
|----------|---------|------------------|
| 100% | Full field | Initial exploration |
| 50-99% | Remove strongest | Focus on weak regions |
| 10-50% | Mid-range only | Examine moderate strengths |
| 1-10% | Weakest only | Find subtle features |

### 8.5 Performance Impact

**Benefits:**
- Fewer arrows to render ? faster
- Reduced visual complexity
- Better frame rates for large fields

**Example:**
- 10,000 vectors, cutoff 50% ? renders 5,000 arrows
- 10,000 vectors, cutoff 10% ? renders 1,000 arrows

---

## 9. Color Coding

### 9.1 Magnitude-Based Gradient

**When "Color by magnitude" is enabled:**

**Color Scale:** Blue ? Cyan ? Green ? Yellow ? Red

**Mapping (with cutoff awareness):**
```
t = (magnitude - minMag) / (cutoffMag - minMag)  // Normalize to [0,1]

if (t < 0.25):      Blue ? Cyan     (0,g,255) where g: 0?255
elif (t < 0.5):     Cyan ? Green    (0,255,b) where b: 255?0
elif (t < 0.75):    Green ? Yellow  (r,255,0) where r: 0?255
else:               Yellow ? Red    (255,g,0) where g: 255?0
```

**Visual Interpretation:**
| Color | Magnitude (relative to cutoff) | Typical Meaning |
|-------|--------------------------------|-----------------|
| Blue | Minimum | Weakest vectors |
| Cyan | Low (25%) | Below average |
| Green | Medium (50%) | Average |
| Yellow | High (75%) | Above average |
| Red | At cutoff | Strongest visible |

### 9.2 Solid Color Mode

**When "Color by magnitude" is disabled:**
- All arrows use the selected color from dropdown
- Available: Blue, Red, Green, Orange, Purple, Cyan
- Default: Blue

---

## 10. Interactive Camera Control

### 10.1 Mouse Controls

| Action | Input | Technical Details |
|--------|-------|-------------------|
| Rotate View | Left Mouse + Drag | Orbital rotation around look-at point |
| Pan View | Right Mouse + Drag | Translate camera in view plane |
| Zoom | Mouse Wheel Scroll | Dolly camera along view direction |

### 10.2 Keyboard Controls

| Action | Keys | Behavior |
|--------|------|----------|
| Pan Left | Left Arrow | Move camera left |
| Pan Right | Right Arrow | Move camera right |
| Pan Up | Up Arrow | Move camera up |
| Pan Down | Down Arrow | Move camera down |
| Zoom In | + or = | Move camera closer |
| Zoom Out | - or _ | Move camera farther |

### 10.3 Camera Buttons

**Look at Center:**
- Reorients camera to look at field center
- Maintains camera distance
- Useful after rotation/pan

**Reset Camera:**
- Returns to initial position and orientation
- Recalculates based on current data bounds
- Resets up vector to (0, 1, 0)

---

## 11. Interactive Features

### 11.1 Title Editing
- Edit title in sidebar TextBox
- Changes appear immediately in top title display
- Multi-line titles supported

### 11.2 Magnitude Scaling
- Adjust slider in real-time
- Range: 0.1× to 5.0× of optimal scale
- Useful for fields with large magnitude variations

### 11.3 Arrow Width Control
- Adjust arrow thickness independently
- Thinner arrows: Better for dense fields
- Thicker arrows: Better for sparse fields or presentations

### 11.4 Magnitude Cutoff Adjustment ?
- **Real-time filtering** of high-magnitude vectors
- Slider OR text input (synced)
- Immediate scene redraw
- Shows count of visible vectors

### 11.5 Vector Normalization
- When enabled: All vectors same length
- Shows **direction** only, ignores magnitude
- Arrow dimensions still scale with avg magnitude

### 11.6 Bounding Box Toggle
- Shows/hides field boundary
- Useful for spatial reference
- Gray wire-frame style

---

## 12. Error Handling

### 12.1 File Errors

| Error | Message |
|-------|---------|
| No file specified | "No file name specified." |
| File not found | "File does not exist: {path}" |
| Unsupported format | "Unsupported format: {type}" |
| Parse error | "Error loading file: {details}" |

### 12.2 Data Edge Cases

| Case | Handling |
|------|----------|
| Zero vectors | Skipped in rendering |
| Zero magnitude | Skipped (not rendered) |
| Single point | Adds default bounds (±1) |
| All zero magnitudes | Default scale = 1.0, minMag=0, maxMag=1 |
| NaN/Infinity | Skipped |
| Cutoff < minMag | Clamped to minMag |
| Cutoff > maxMag | Clamped to maxMag |

---

## 13. Implementation Architecture

### 13.1 Key Classes

| Class | Responsibility |
|-------|----------------|
| `MainWindow` | UI coordination, rendering, cutoff management |
| `Vector3Repr` | Stores position + vector data |
| `WorldCameraMouseHelper` | 3D camera control logic |
| `Geometries` | 3D geometry creation (arrows, spheres, cylinders) |
| `Utils` | Scene utilities (coordinate system drawing) |

### 13.2 Data Storage

```csharp
class Vector3Repr
{
    Vector3Cartesian Pos;   // (x, y, z) position
    Vector3Cartesian Vec;   // (vx, vy, vz) vector
}
```

### 13.3 Rendering Pipeline

```
1. Load Data
   ??> Parse file ? List<Vector3Repr>
   
2. Initialize Scene
   ??> CalculateDataBounds()
   ??> CalculateOptimalScale()
   ??> SetupCamera()
   ??> UpdateUITextBoxes()
   ??> InitializeCutoffSlider()
   
3. Redraw Process (on any change)
   ??> Clear scene
   ??> Add lighting
   ??> Draw coordinate axes
   ??> If showBoundingBox: DrawBoundingBox()
   ??> DrawVectors() with cutoff filter
       ??> For each vector:
           ??> Calculate magnitude
           ??> If magnitude > cutoff: skip
           ??> Calculate effective scale
           ??> Create arrow geometry
           ??> Determine color (relative to cutoff)
           ??> Position and orient arrow
           ??> Add to scene
   ??> Update vectors shown count
```

### 13.4 Key Algorithms

**Automatic Scaling (Cube Root Based):**
```csharp
double xRange = _xMax - _xMin;
double yRange = _yMax - _yMin;
double zRange = _zMax - _zMin;
int estimatedGridSize = (int)Math.Pow(_vectors.Count, 1.0/3.0);
estimatedGridSize = Math.Max(2, estimatedGridSize);

double avgSpacingX = xRange / (estimatedGridSize - 1);
double avgSpacingY = yRange / (estimatedGridSize - 1);
double avgSpacingZ = zRange / (estimatedGridSize - 1);
double avgSpacing = Math.Min(avgSpacingX, Math.Min(avgSpacingY, avgSpacingZ));

_optimalVectorScale = (0.5 * avgSpacing) / _avgMagnitude;
_optimalVectorScale = Math.Clamp(_optimalVectorScale, 0.001, 100000);
```

**Cutoff Filtering:**
```csharp
int count = 0;
foreach (var vec in _vectors)
{
    double magnitude = vec.Vec.Norm();
    
    // Skip vectors above cutoff
    if (magnitude > _magnitudeCutoff)
        continue;
    
    if (DrawVectorArrow(vec, effectiveScale))
        count++;
}
txtVectorsShown.Text = $"Showing: {count} / {_vectors.Count}";
```

**Linear Arrow Scaling:**
```csharp
double magnitude = vec.Vec.Norm();
double length;

if (_normalizeVectors)
{
    length = effectiveScale * _avgMagnitude;  // All same length
}
else
{
    length = effectiveScale * magnitude;  // Linear scaling
}

double baseWidth = length * 0.08 * _arrowWidthScale;
double headWidth = length * 0.18 * _arrowWidthScale;
double headLength = length * 0.25;
```

---

## 14. Performance Considerations

### 14.1 Vector Count Impact

| Num Vectors | Grid Size | Arrows Rendered | Rendering Speed | Recommendations |
|-------------|-----------|-----------------|-----------------|-----------------|
| 100 | 5×5×5 (4³) | 100 | Instant | Any settings |
| 1,000 | 10×10×10 | 1,000 | Very fast | Any settings |
| 8,000 | 20×20×20 | 8,000 | Fast | Default settings |
| 27,000 | 30×30×30 | 27,000 | Moderate | Use cutoff |
| 64,000 | 40×40×40 | 64,000 | Slow | Use cutoff heavily |

### 14.2 Cutoff as Performance Tool

**Effective Strategies:**

| Field Size | Cutoff % | Arrows Rendered | Frame Rate Improvement |
|------------|----------|-----------------|------------------------|
| 27,000 | 100% | 27,000 | Baseline |
| 27,000 | 50% | ~13,500 | ~2× faster |
| 27,000 | 25% | ~6,750 | ~4× faster |
| 27,000 | 10% | ~2,700 | ~10× faster |

### 14.3 Optimization Strategies

**For Large Fields (>10,000 vectors):**
- Start with low cutoff (10-50%)
- Gradually increase to explore
- Reduce arrow width to minimum (0.2)
- Disable bounding box if not needed

**For Dense Fields:**
- Use cutoff to thin out arrows
- Normalize vectors to uniform length
- Use solid color (faster than gradient)

---

## 15. Extensibility Points

### 15.1 Enhanced Cutoff Features
- **Range cutoff**: Show only vectors with magnitude ? [min, max]
- **Inverted cutoff**: Show only vectors above threshold
- **Spatial cutoff**: Filter by region (box, sphere)
- **Component cutoff**: Filter by vx, vy, or vz ranges

### 15.2 Additional Arrow Styles
- Variable arrow thickness by magnitude
- Streamlines/tubes instead of arrows
- Glyph alternatives (cones, hedgehogs)
- Arrow transparency by magnitude

### 15.3 Field Analysis Tools
- Divergence/curl visualization
- Critical point detection
- Field line tracing
- Isosurface of magnitude

### 15.4 Export Features
- Export visible arrows to file
- Export animation (rotating view)
- Export magnitude histogram
- Export cutoff-filtered data

---

## 16. Platform Porting Notes

### 16.1 Core Components to Port
1. **File Loader** - Pure logic
2. **Data Storage** - Vector3Repr class
3. **Scaling Algorithm** - Cube root based
4. **Cutoff Logic** - Filtering system
5. **3D Rendering** - Platform-specific

### 16.2 Platform-Specific Elements

| WPF Element | Qt Equivalent | OpenGL Equivalent |
|-------------|---------------|-------------------|
| Viewport3D | Qt3D or QOpenGLWidget | OpenGL context |
| PerspectiveCamera | QCamera | gluPerspective() |
| GeometryModel3D | Qt3DCore::QEntity | Display list/VBO |
| DiffuseMaterial | QDiffuseMapMaterial | glMaterial() |
| Transform3D | QTransform | glMultMatrix() |
| DirectionalLight | QDirectionalLight | glLight() |

### 16.3 Arrow Geometry Porting

**WPF (current):**
```csharp
MeshGeometry3D arrow = Geometries.CreateVectorArrow(...);
// Pre-computed mesh with vertices, normals, indices
```

**Qt:**
```cpp
Qt3DExtras::QConeMesh shaft;
Qt3DExtras::QConeMesh head;
// Combine and transform
```

**OpenGL:**
```cpp
// Generate cylinder + cone using GLU
gluCylinder(quadric, baseWidth, baseWidth, shaftLength, slices, stacks);
gluCylinder(quadric, headWidth, 0, headLength, slices, stacks);
```

### 16.4 Cutoff Implementation (Universal)

```cpp
std::vector<Arrow> visibleArrows;
for (const auto& vec : vectors) {
    double mag = vec.magnitude();
    if (mag <= cutoffValue) {
        visibleArrows.push_back(createArrow(vec, ...));
    }
}
render(visibleArrows);
```

---

## Appendix A: Sample Data Files

### A.1 Simple Radial Field
```
VECTOR_FIELD_3D_CARTESIAN
radial_3d
0 0 0 0 0 0
1 0 0 1 0 0
0 1 0 0 1 0
0 0 1 0 0 1
-1 0 0 -1 0 0
0 -1 0 0 -1 0
0 0 -1 0 0 -1
```

### A.2 Gravitational Field (Point Mass)
```
VECTOR_FIELD_3D_CARTESIAN
gravity_point_mass
-10 -10 -10 0.0577 0.0577 0.0577
-10 -10 0 0.0707 0.0707 0
-10 0 -10 0.0707 0 0.0707
-10 0 0 0.1 0 0
0 -10 -10 0 0.0707 0.0707
0 -10 0 0 0.1 0
0 0 -10 0 0 0.1
```

### A.3 Magnetic Dipole
```
VECTOR_FIELD_3D_CARTESIAN
magnetic_dipole
-5 -5 -5 0.02 0.01 -0.03
-5 -5 0 0.03 0.02 0
-5 0 -5 0.04 0 -0.02
...
```

---

## Appendix B: Common 3D Vector Fields

| Field Type | Formula | Description |
|------------|---------|-------------|
| Radial | V = (x, y, z) | Outward from origin |
| Rotational | V = (-y, x, 0) | Rotation about Z-axis |
| Gradient | V = ?f | Steepest ascent direction |
| Constant | V = (a, b, c) | Uniform flow |
| Dipole | V = 3(m·r)r/r? - m/r³ | Magnetic/electric dipole |
| Gravitational | V = -GM r/r³ | Newton's law |

---

## Appendix C: Magnitude Cutoff Examples

### C.1 Use Case: Gravitational Field Around Sun

**Scenario:**
- Vector field from Sun + 8 planets
- Sun's gravity dominates (very large magnitude near Sun)
- Want to see planets' contributions

**Solution:**
1. Load full field (cutoff 100%)
2. Observe Sun's vectors dominate (red everywhere near Sun)
3. Reduce cutoff to 50% ? see intermediate region
4. Reduce cutoff to 10% ? see planets' individual fields clearly

### C.2 Use Case: Fluid Flow with Singularities

**Scenario:**
- CFD simulation with flow around obstacles
- Singularities at sharp corners (very high velocity)
- Want to visualize bulk flow

**Solution:**
1. Cutoff 100% ? arrows at corners too long, obscure everything
2. Cutoff 80% ? remove extreme outliers
3. Adjust until bulk flow structure is clear

### C.3 Use Case: Electromagnetic Field

**Scenario:**
- E-field from multiple charges
- Near-field is very strong, far-field is weak
- Want to see field structure at different distances

**Solution:**
1. High cutoff (90-100%) ? see near-field structure
2. Low cutoff (10-20%) ? see far-field structure
3. Animate cutoff from high to low ? "zoom out" visualization

---

## Appendix D: Arrow Dimensions

### D.1 Dimension Relationships

```
Given:
    length = effectiveScale × magnitude (or avgMagnitude if normalized)
    arrowWidthScale = user slider value (0.2 to 3.0)

Calculate:
    baseWidth = length × 0.08 × arrowWidthScale
    headWidth = length × 0.18 × arrowWidthScale
    headLength = length × 0.25

Constraints:
    baseWidth ? 0.1
    headWidth ? 2 × baseWidth
```

### D.2 Proportions

| Component | Proportion to Length |
|-----------|----------------------|
| Shaft length | 75% (length - headLength) |
| Head length | 25% |
| Base width | 8% (before width scale) |
| Head width | 18% (before width scale) |

### D.3 Width Scale Effects

| arrowWidthScale | Visual Effect |
|-----------------|---------------|
| 0.2 | Very thin, thread-like |
| 0.5 | Thin, suitable for dense fields |
| 1.0 | Default, balanced |
| 2.0 | Thick, suitable for presentations |
| 3.0 | Very thick, bold emphasis |
