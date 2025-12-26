using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Media3D;

using MML;
using WPF3DHelperLib;

namespace MML_VectorField3D_Visualizer
{
  /// <summary>
  /// Main window for the 3D Vector Field Visualizer application.
  /// </summary>
  public partial class MainWindow : Window
  {
    private readonly WorldCameraMouseHelper _cameraHelper = new WorldCameraMouseHelper();
    private readonly List<Vector3Repr> _vectors = new List<Vector3Repr>();
    private Model3DGroup _modelGroup = new Model3DGroup();
    private ModelVisual3D _modelVisual = new ModelVisual3D();

    private string _title = "";
    private bool _isUpdatingTitle = false;
    private bool _isInitialized = false;
    private bool _isUpdatingCutoff = false;

    // Data bounds
    private double _xMin, _xMax, _yMin, _yMax, _zMin, _zMax;
    private double _minMagnitude, _maxMagnitude, _avgMagnitude;
    private Point3D _centerPoint;

    // Calculated optimal scale
    private double _optimalVectorScale = 1.0;

    // Display options
    private bool _colorByMagnitude = true;
    private bool _normalizeVectors = false;
    private bool _showBoundingBox = false;
    private double _magnitudeScale = 1.0;
    private double _arrowWidthScale = 1.0;
    private Color _arrowColor = Colors.Blue;

    // Magnitude cutoff - vectors with magnitude > cutoff are not displayed
    private double _magnitudeCutoff = double.MaxValue;
    private double _cutoffPercent = 100.0;

    // Initial camera position for reset
    private Point3D _initialCameraPosition;

    /// <summary>
    /// Initializes a new instance of the <see cref="MainWindow"/> class.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      Loaded += MainWindow_Loaded;

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }

      var fileName = args[1];
      LoadData(fileName);
    }

    /// <summary>
    /// Handles the window Loaded event to initialize the visualization.
    /// </summary>
    private void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
      if (_vectors.Count == 0) return;

      CalculateDataBounds();
      CalculateOptimalScale();
      SetupCamera();
      UpdateUITextBoxes();
      InitializeCutoffSlider();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      _isInitialized = true;
      Redraw();
    }

    /// <summary>
    /// Initializes the cutoff slider based on magnitude range.
    /// </summary>
    private void InitializeCutoffSlider()
    {
      _isUpdatingCutoff = true;

      // Set slider range based on actual magnitude range
      sliderCutoff.Minimum = 0;
      sliderCutoff.Maximum = 100;
      sliderCutoff.Value = 100;

      _magnitudeCutoff = _maxMagnitude;
      _cutoffPercent = 100.0;

      UpdateCutoffDisplay();

      _isUpdatingCutoff = false;
    }

    /// <summary>
    /// Updates the cutoff display text boxes.
    /// </summary>
    private void UpdateCutoffDisplay()
    {
      txtCutoffValue.Text = FormatValue(_magnitudeCutoff);
      txtCutoffPercent.Text = $"{_cutoffPercent:F0}%";
    }

    /// <summary>
    /// Calculates data bounds and magnitude statistics from loaded vectors.
    /// </summary>
    private void CalculateDataBounds()
    {
      if (_vectors.Count == 0) return;

      _xMin = _xMax = _vectors[0].Pos.X;
      _yMin = _yMax = _vectors[0].Pos.Y;
      _zMin = _zMax = _vectors[0].Pos.Z;
      _minMagnitude = double.MaxValue;
      _maxMagnitude = double.MinValue;
      double totalMagnitude = 0;
      int count = 0;

      foreach (var v in _vectors)
      {
        _xMin = Math.Min(_xMin, v.Pos.X);
        _xMax = Math.Max(_xMax, v.Pos.X);
        _yMin = Math.Min(_yMin, v.Pos.Y);
        _yMax = Math.Max(_yMax, v.Pos.Y);
        _zMin = Math.Min(_zMin, v.Pos.Z);
        _zMax = Math.Max(_zMax, v.Pos.Z);

        double magnitude = v.Vec.Norm();
        if (magnitude > 1e-15)
        {
          _minMagnitude = Math.Min(_minMagnitude, magnitude);
          _maxMagnitude = Math.Max(_maxMagnitude, magnitude);
          totalMagnitude += magnitude;
          count++;
        }
      }

      if (count > 0)
      {
        _avgMagnitude = totalMagnitude / count;
      }
      else
      {
        _avgMagnitude = 1.0;
        _minMagnitude = 0;
        _maxMagnitude = 1;
      }

      _centerPoint = new Point3D(
        (_xMin + _xMax) / 2,
        (_yMin + _yMax) / 2,
        (_zMin + _zMax) / 2);

      // Initialize cutoff to max magnitude
      _magnitudeCutoff = _maxMagnitude;
    }

    /// <summary>
    /// Calculates the optimal vector scale based on data analysis.
    /// Uses linear scaling based on average magnitude and grid spacing.
    /// </summary>
    private void CalculateOptimalScale()
    {
      if (_vectors.Count == 0 || _maxMagnitude < 1e-15) return;

      // Calculate average spacing between vector positions
      double xRange = _xMax - _xMin;
      double yRange = _yMax - _yMin;
      double zRange = _zMax - _zMin;

      // Estimate grid spacing
      int estimatedGridSize = (int)Math.Pow(_vectors.Count, 1.0 / 3.0);
      estimatedGridSize = Math.Max(2, estimatedGridSize);

      double avgSpacingX = xRange / (estimatedGridSize - 1);
      double avgSpacingY = yRange / (estimatedGridSize - 1);
      double avgSpacingZ = zRange / (estimatedGridSize - 1);
      double avgSpacing = Math.Min(avgSpacingX, Math.Min(avgSpacingY, avgSpacingZ));

      // Scale so that average vector is about 50% of grid spacing
      if (_avgMagnitude > 1e-15)
      {
        _optimalVectorScale = (0.5 * avgSpacing) / _avgMagnitude;
      }
      else
      {
        _optimalVectorScale = 1.0;
      }

      _optimalVectorScale = Math.Clamp(_optimalVectorScale, 0.001, 100000);
    }

    /// <summary>
    /// Sets up the camera based on data bounds.
    /// </summary>
    private void SetupCamera()
    {
      double xRange = _xMax - _xMin;
      double yRange = _yMax - _yMin;
      double zRange = _zMax - _zMin;
      double maxRange = Math.Max(xRange, Math.Max(yRange, zRange));

      double distance = maxRange * 2.0;
      _initialCameraPosition = new Point3D(
        _centerPoint.X + distance * 0.6,
        _centerPoint.Y + distance * 0.4,
        _centerPoint.Z + distance * 0.6);

      _cameraHelper.InitCamera(_initialCameraPosition);
      _cameraHelper.InitLights(_modelGroup);
      myViewport3D.Camera = _cameraHelper._myCamera;

      // Look at center
      _cameraHelper._myCamera.LookDirection = new Vector3D(
        _centerPoint.X - _initialCameraPosition.X,
        _centerPoint.Y - _initialCameraPosition.Y,
        _centerPoint.Z - _initialCameraPosition.Z);
    }

    /// <summary>
    /// Updates UI text boxes with current field information.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtXRange.Text = $"{FormatValue(_xMin)} to {FormatValue(_xMax)}";
      txtYRange.Text = $"{FormatValue(_yMin)} to {FormatValue(_yMax)}";
      txtZRange.Text = $"{FormatValue(_zMin)} to {FormatValue(_zMax)}";
      txtNumVectors.Text = _vectors.Count.ToString();
      txtMaxMagnitude.Text = FormatValue(_maxMagnitude);
      txtMinMagnitude.Text = FormatValue(_minMagnitude);
      txtAvgMagnitude.Text = FormatValue(_avgMagnitude);
    }

    /// <summary>
    /// Formats a numeric value for display.
    /// </summary>
    private string FormatValue(double value)
    {
      if (double.IsInfinity(value) || double.IsNaN(value)) return "N/A";
      if (Math.Abs(value) >= 10000 || (Math.Abs(value) > 0 && Math.Abs(value) < 0.001))
        return value.ToString("E3");
      if (Math.Abs(value - Math.Round(value)) < 1e-10)
        return ((long)Math.Round(value)).ToString();
      return value.ToString("G4");
    }

    /// <summary>
    /// Redraws the entire 3D visualization.
    /// </summary>
    private void Redraw()
    {
      if (_vectors.Count == 0 || !_isInitialized) return;

      _modelGroup = new Model3DGroup();
      _cameraHelper.InitLights(_modelGroup);

      double axisLength = Math.Max(_xMax - _xMin, Math.Max(_yMax - _yMin, _zMax - _zMin)) * 0.6;
      Utils.DrawCoordSystem(_modelGroup, 0.5, axisLength);

      if (_showBoundingBox)
      {
        DrawBoundingBox();
      }

      int vectorsDrawn = DrawVectors();

      // Update vectors shown count
      txtVectorsShown.Text = $"Showing: {vectorsDrawn} / {_vectors.Count}";

      _modelVisual.Content = _modelGroup;

      if (!myViewport3D.Children.Contains(_modelVisual))
      {
        myViewport3D.Children.Add(_modelVisual);
      }
    }

    /// <summary>
    /// Draws a bounding box around the data.
    /// </summary>
    private void DrawBoundingBox()
    {
      var boxColor = Colors.Gray;

      // Draw 12 edges of the bounding box using thin cylinders
      // Bottom face
      AddBoxLine(_xMin, _yMin, _zMin, _xMax, _yMin, _zMin, boxColor);
      AddBoxLine(_xMax, _yMin, _zMin, _xMax, _yMax, _zMin, boxColor);
      AddBoxLine(_xMax, _yMax, _zMin, _xMin, _yMax, _zMin, boxColor);
      AddBoxLine(_xMin, _yMax, _zMin, _xMin, _yMin, _zMin, boxColor);

      // Top face
      AddBoxLine(_xMin, _yMin, _zMax, _xMax, _yMin, _zMax, boxColor);
      AddBoxLine(_xMax, _yMin, _zMax, _xMax, _yMax, _zMax, boxColor);
      AddBoxLine(_xMax, _yMax, _zMax, _xMin, _yMax, _zMax, boxColor);
      AddBoxLine(_xMin, _yMax, _zMax, _xMin, _yMin, _zMax, boxColor);

      // Vertical edges
      AddBoxLine(_xMin, _yMin, _zMin, _xMin, _yMin, _zMax, boxColor);
      AddBoxLine(_xMax, _yMin, _zMin, _xMax, _yMin, _zMax, boxColor);
      AddBoxLine(_xMax, _yMax, _zMin, _xMax, _yMax, _zMax, boxColor);
      AddBoxLine(_xMin, _yMax, _zMin, _xMin, _yMax, _zMax, boxColor);
    }

    /// <summary>
    /// Adds a line segment to the model group for bounding box.
    /// </summary>
    private void AddBoxLine(double x1, double y1, double z1, double x2, double y2, double z2, Color color)
    {
      // Calculate line length and direction
      double dx = x2 - x1;
      double dy = y2 - y1;
      double dz = z2 - z1;
      double length = Math.Sqrt(dx * dx + dy * dy + dz * dz);

      if (length < 1e-10) return;

      // Use a thin cylinder
      double maxRange = Math.Max(_xMax - _xMin, Math.Max(_yMax - _yMin, _zMax - _zMin));
      double radius = maxRange * 0.002;

      MeshGeometry3D mesh = Geometries.CreateCylinder(radius, length, 6, 1);
      var material = new DiffuseMaterial(new SolidColorBrush(color));
      var model = new GeometryModel3D(mesh, material);

      // Calculate rotation and translation
      var currentDir = new Vector3Cartesian(0, 0, 1);
      var newDir = new Vector3Cartesian(dx / length, dy / length, dz / length);

      var cross = Vector3Cartesian.VectorProd(currentDir, newDir);
      double angle = Vector3Cartesian.VectorAngle(currentDir, newDir) / Math.PI * 180;

      // Midpoint for positioning
      double midX = (x1 + x2) / 2;
      double midY = (y1 + y2) / 2;
      double midZ = (z1 + z2) / 2 - length / 2;

      Transform3D transform;
      if (cross.Norm() < 1e-10)
      {
        if (angle < 90)
        {
          transform = new TranslateTransform3D(x1, y1, z1);
        }
        else
        {
          var group = new Transform3DGroup();
          group.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(1, 0, 0), 180)));
          group.Children.Add(new TranslateTransform3D(x1, y1, z1 + length));
          transform = group;
        }
      }
      else
      {
        var rot = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(cross.X, cross.Y, cross.Z), angle));
        var trans = new TranslateTransform3D(x1, y1, z1);
        model.Transform = new MatrixTransform3D(rot.Value * trans.Value);
        _modelGroup.Children.Add(model);
        return;
      }

      model.Transform = transform;
      _modelGroup.Children.Add(model);
    }

    /// <summary>
    /// Draws all vector arrows, applying magnitude cutoff.
    /// Returns the number of vectors drawn.
    /// </summary>
    private int DrawVectors()
    {
      int count = 0;
      double effectiveScale = _optimalVectorScale * _magnitudeScale;

      foreach (var vec in _vectors)
      {
        double magnitude = vec.Vec.Norm();

        // Skip vectors above cutoff magnitude
        if (magnitude > _magnitudeCutoff)
          continue;

        if (DrawVectorArrow(vec, effectiveScale))
          count++;
      }

      return count;
    }

    /// <summary>
    /// Draws a single vector arrow with linear scaling.
    /// Returns true if the vector was drawn.
    /// </summary>
    private bool DrawVectorArrow(Vector3Repr vec, double effectiveScale)
    {
      double magnitude = vec.Vec.Norm();
      if (magnitude < 1e-15) return false;

      // Get unit direction
      double ux = vec.Vec.X / magnitude;
      double uy = vec.Vec.Y / magnitude;
      double uz = vec.Vec.Z / magnitude;

      // Calculate arrow length - LINEAR scaling
      double length;
      if (_normalizeVectors)
      {
        // All arrows same length (use average magnitude as reference)
        length = effectiveScale * _avgMagnitude;
      }
      else
      {
        // Linear scaling - length proportional to magnitude
        length = effectiveScale * magnitude;
      }

      if (length < 1e-10) return false;

      // Arrow dimensions - all proportional to length
      double baseWidth = length * 0.08 * _arrowWidthScale;
      double headWidth = length * 0.18 * _arrowWidthScale;  // Head width proportional to length
      double headLength = length * 0.25;

      // Ensure minimum visible size
      baseWidth = Math.Max(baseWidth, 0.1);
      headWidth = Math.Max(headWidth, baseWidth * 2.0);

      // Determine color based on magnitude relative to cutoff (not max)
      Color color;
      if (_colorByMagnitude && _magnitudeCutoff > _minMagnitude)
      {
        // Color based on position within [min, cutoff] range
        double t = (magnitude - _minMagnitude) / (_magnitudeCutoff - _minMagnitude);
        t = Math.Clamp(t, 0, 1);
        color = GetMagnitudeColor(t);
      }
      else
      {
        color = _arrowColor;
      }

      // Create arrow geometry
      MeshGeometry3D arrowMesh = Geometries.CreateVectorArrow(baseWidth, length, 8, 5, headWidth, headLength);
      var material = new DiffuseMaterial(new SolidColorBrush(color));
      GeometryModel3D arrowModel = new GeometryModel3D(arrowMesh, material);

      // Calculate rotation to align with vector direction
      var currentDir = new Vector3Cartesian(0, 0, 1);
      var newDir = new Vector3Cartesian(ux, uy, uz);

      var cross = Vector3Cartesian.VectorProd(currentDir, newDir);
      double angle = Vector3Cartesian.VectorAngle(currentDir, newDir) / Math.PI * 180;

      Transform3D transform;
      if (cross.Norm() < 1e-10)
      {
        if (angle < 90)
        {
          transform = new TranslateTransform3D(vec.Pos.X, vec.Pos.Y, vec.Pos.Z);
        }
        else
        {
          var rotGroup = new Transform3DGroup();
          rotGroup.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(1, 0, 0), 180)));
          rotGroup.Children.Add(new TranslateTransform3D(vec.Pos.X, vec.Pos.Y, vec.Pos.Z));
          transform = rotGroup;
        }
      }
      else
      {
        var rot = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(cross.X, cross.Y, cross.Z), angle));
        var trans = new TranslateTransform3D(vec.Pos.X, vec.Pos.Y, vec.Pos.Z);
        arrowModel.Transform = new MatrixTransform3D(rot.Value * trans.Value);
        _modelGroup.Children.Add(arrowModel);
        return true;
      }

      arrowModel.Transform = transform;
      _modelGroup.Children.Add(arrowModel);
      return true;
    }

    /// <summary>
    /// Gets a color based on normalized magnitude (0 to 1).
    /// Uses a blue-cyan-green-yellow-red gradient.
    /// </summary>
    private Color GetMagnitudeColor(double t)
    {
      t = Math.Clamp(t, 0, 1);

      byte r, g, b;

      if (t < 0.25)
      {
        double localT = t / 0.25;
        r = 0;
        g = (byte)(255 * localT);
        b = 255;
      }
      else if (t < 0.5)
      {
        double localT = (t - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = (byte)(255 * (1 - localT));
      }
      else if (t < 0.75)
      {
        double localT = (t - 0.5) / 0.25;
        r = (byte)(255 * localT);
        g = 255;
        b = 0;
      }
      else
      {
        double localT = (t - 0.75) / 0.25;
        r = 255;
        g = (byte)(255 * (1 - localT));
        b = 0;
      }

      return Color.FromRgb(r, g, b);
    }

    /// <summary>
    /// Loads vector field data from a file.
    /// </summary>
    private bool LoadData(string fileName)
    {
      if (!File.Exists(fileName))
      {
        MessageBox.Show($"File does not exist: {fileName}");
        return false;
      }

      try
      {
        string[] lines = File.ReadAllLines(fileName);
        string type = lines[0].Trim();

        if (type != "VECTOR_FIELD_3D_CARTESIAN")
        {
          MessageBox.Show($"Unsupported format: {type}");
          return false;
        }

        _title = lines[1].Trim();

        for (int i = 2; i < lines.Length; i++)
        {
          if (string.IsNullOrWhiteSpace(lines[i])) continue;

          string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 6) continue;

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);
          double z = double.Parse(parts[2], CultureInfo.InvariantCulture);
          double vx = double.Parse(parts[3], CultureInfo.InvariantCulture);
          double vy = double.Parse(parts[4], CultureInfo.InvariantCulture);
          double vz = double.Parse(parts[5], CultureInfo.InvariantCulture);

          _vectors.Add(new Vector3Repr(
            new Vector3Cartesian(x, y, z),
            new Vector3Cartesian(vx, vy, vz)));
        }

        return true;
      }
      catch (Exception e)
      {
        MessageBox.Show($"Error loading file: {e.Message}");
        return false;
      }
    }

    /// <summary>
    /// Gets a Color from a ComboBox selection.
    /// </summary>
    private Color GetColorFromComboBox(ComboBox comboBox)
    {
      if (comboBox.SelectedItem is ComboBoxItem item)
      {
        return item.Content?.ToString() switch
        {
          "Blue" => Colors.Blue,
          "Red" => Colors.Red,
          "Green" => Colors.Green,
          "Orange" => Colors.Orange,
          "Purple" => Colors.Purple,
          "Cyan" => Colors.Cyan,
          _ => Colors.Blue
        };
      }
      return Colors.Blue;
    }

    #region Event Handlers

    private void OnTitleTextChanged(object sender, TextChangedEventArgs e)
    {
      if (_isUpdatingTitle) return;
      _title = txtEditableTitle.Text;
      txtTitle.Text = _title;
    }

    private void sliderMagnitudeScale_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if (sliderMagnitudeScale == null || txtMagnitudeScale == null) return;
      _magnitudeScale = sliderMagnitudeScale.Value;
      txtMagnitudeScale.Text = _magnitudeScale.ToString("F2");
      if (_isInitialized) Redraw();
    }

    private void sliderArrowWidth_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if (sliderArrowWidth == null || txtArrowWidth == null) return;
      _arrowWidthScale = sliderArrowWidth.Value;
      txtArrowWidth.Text = _arrowWidthScale.ToString("F2");
      if (_isInitialized) Redraw();
    }

    private void sliderCutoff_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if (sliderCutoff == null || _isUpdatingCutoff || !_isInitialized) return;

      _isUpdatingCutoff = true;

      // Slider goes from 0 to 100 (percentage)
      _cutoffPercent = sliderCutoff.Value;

      // Calculate actual cutoff magnitude from percentage
      // Use linear interpolation between min and max magnitude
      _magnitudeCutoff = _minMagnitude + (_maxMagnitude - _minMagnitude) * (_cutoffPercent / 100.0);

      UpdateCutoffDisplay();
      _isUpdatingCutoff = false;

      Redraw();
    }

    private void txtCutoffValue_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
      {
        // Try to parse the entered value
        if (double.TryParse(txtCutoffValue.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out double newCutoff))
        {
          _isUpdatingCutoff = true;

          // Clamp to valid range
          newCutoff = Math.Clamp(newCutoff, _minMagnitude, _maxMagnitude);
          _magnitudeCutoff = newCutoff;

          // Update percentage and slider
          if (_maxMagnitude > _minMagnitude)
          {
            _cutoffPercent = ((_magnitudeCutoff - _minMagnitude) / (_maxMagnitude - _minMagnitude)) * 100.0;
          }
          else
          {
            _cutoffPercent = 100.0;
          }

          sliderCutoff.Value = _cutoffPercent;
          UpdateCutoffDisplay();

          _isUpdatingCutoff = false;

          if (_isInitialized) Redraw();
        }
        else
        {
          // Reset to current value if parse failed
          UpdateCutoffDisplay();
        }

        e.Handled = true;
      }
    }

    private void OnColorByMagnitudeChanged(object sender, RoutedEventArgs e)
    {
      _colorByMagnitude = chkColorByMagnitude?.IsChecked ?? true;
      if (_isInitialized) Redraw();
    }

    private void OnNormalizeVectorsChanged(object sender, RoutedEventArgs e)
    {
      _normalizeVectors = chkNormalizeVectors?.IsChecked ?? false;
      if (_isInitialized) Redraw();
    }

    private void OnShowBoundingBoxChanged(object sender, RoutedEventArgs e)
    {
      _showBoundingBox = chkShowBoundingBox?.IsChecked ?? false;
      if (_isInitialized) Redraw();
    }

    private void cmbArrowColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if (cmbArrowColor == null) return;
      _arrowColor = GetColorFromComboBox(cmbArrowColor);
      if (_isInitialized && !_colorByMagnitude) Redraw();
    }

    private void btnLookAtCenter_Click(object sender, RoutedEventArgs e)
    {
      if (_vectors.Count == 0) return;
      _cameraHelper._myCamera.LookDirection = new Vector3D(
        _centerPoint.X - _cameraHelper._myCamera.Position.X,
        _centerPoint.Y - _cameraHelper._myCamera.Position.Y,
        _centerPoint.Z - _cameraHelper._myCamera.Position.Z);
    }

    private void btnResetCamera_Click(object sender, RoutedEventArgs e)
    {
      if (_vectors.Count == 0) return;
      _cameraHelper._myCamera.Position = _initialCameraPosition;
      _cameraHelper._myCamera.LookDirection = new Vector3D(
        _centerPoint.X - _initialCameraPosition.X,
        _centerPoint.Y - _initialCameraPosition.Y,
        _centerPoint.Z - _initialCameraPosition.Z);
      _cameraHelper._myCamera.UpDirection = new Vector3D(0, 1, 0);
    }

    #endregion

    #region Mouse/Keyboard Handlers

    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _cameraHelper.Window_MouseLeftButtonDown(e.GetPosition(this));
    }

    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _cameraHelper.Window_MouseLeftButtonUp();
    }

    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _cameraHelper.Window_MouseRightButtonDown(e.GetPosition(this));
    }

    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _cameraHelper.Window_MouseRightButtonUp();
    }

    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      _cameraHelper.Window_MouseMove(myViewport3D, e.GetPosition(this), sender, e);
    }

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _cameraHelper.Window_MouseWheel(myViewport3D, sender, e);
    }

    private void Window_KeyDown(object sender, KeyEventArgs e)
    {
      _cameraHelper.Window_KeyDown(myViewport3D, e);
    }

    #endregion
  }
}
