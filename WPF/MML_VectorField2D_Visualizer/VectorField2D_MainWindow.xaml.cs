using MML;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_VectorField2D_Visualizer
{
  /// <summary>
  /// Main window for the 2D Vector Field Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides interactive visualization of 2D vector fields.
  /// It supports loading vector field data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Vector arrows with adjustable scale via slider</description></item>
  ///   <item><description>Automatic scaling based on data analysis</description></item>
  ///   <item><description>Color coding by vector magnitude</description></item>
  ///   <item><description>Option to normalize vectors for uniform display</description></item>
  ///   <item><description>Aspect ratio preservation</description></item>
  ///   <item><description>Editable field title</description></item>
  ///   <item><description>Responsive resizing</description></item>
  /// </list>
  /// </remarks>
  public partial class VectorField2D_MainWindow : Window
  {
    private readonly List<Vector2Repr> _vectors = new List<Vector2Repr>();
    private readonly CoordSystemParams _coordSystemParams = new CoordSystemParams();

    private string _title = "";
    private bool _isUpdatingTitle = false;
    private bool _isInitialized = false;

    // Data bounds
    private double _dataXMin, _dataXMax, _dataYMin, _dataYMax;
    private double _minMagnitude, _maxMagnitude, _avgMagnitude;

    // Calculated optimal scale
    private double _optimalVectorScale = 1.0;

    // Display options
    private bool _preserveAspectRatio = false;
    private bool _normalizeVectors = false;
    private bool _colorByMagnitude = true;
    private double _magnitudeScale = 1.0;
    private double _arrowSize = 8;
    private double _arrowAngle = 25;
    private Color _arrowColor = Colors.Black;

    /// <summary>
    /// Initializes a new instance of the <see cref="VectorField2D_MainWindow"/> class.
    /// </summary>
    public VectorField2D_MainWindow()
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
      InitializeCoordSysParams();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      _isInitialized = true;
      Redraw();
    }

    /// <summary>
    /// Calculates data bounds and magnitude statistics from loaded vectors.
    /// </summary>
    private void CalculateDataBounds()
    {
      if (_vectors.Count == 0) return;

      _dataXMin = double.MaxValue;
      _dataXMax = double.MinValue;
      _dataYMin = double.MaxValue;
      _dataYMax = double.MinValue;
      _minMagnitude = double.MaxValue;
      _maxMagnitude = double.MinValue;
      double totalMagnitude = 0;

      foreach (var v in _vectors)
      {
        _dataXMin = Math.Min(_dataXMin, v.Pos.X);
        _dataXMax = Math.Max(_dataXMax, v.Pos.X);
        _dataYMin = Math.Min(_dataYMin, v.Pos.Y);
        _dataYMax = Math.Max(_dataYMax, v.Pos.Y);

        double magnitude = Math.Sqrt(v.Vec.X * v.Vec.X + v.Vec.Y * v.Vec.Y);
        _minMagnitude = Math.Min(_minMagnitude, magnitude);
        _maxMagnitude = Math.Max(_maxMagnitude, magnitude);
        totalMagnitude += magnitude;
      }

      _avgMagnitude = totalMagnitude / _vectors.Count;
      _coordSystemParams._numPoints = _vectors.Count;
    }

    /// <summary>
    /// Calculates the optimal vector scale based on data analysis.
    /// The goal is to make vectors visible but not overlapping.
    /// </summary>
    private void CalculateOptimalScale()
    {
      if (_vectors.Count == 0 || _maxMagnitude < 1e-10) return;

      // Calculate average spacing between vector positions
      double xRange = _dataXMax - _dataXMin;
      double yRange = _dataYMax - _dataYMin;

      // Estimate grid spacing (assuming roughly uniform distribution)
      int estimatedGridSize = (int)Math.Sqrt(_vectors.Count);
      double avgSpacingX = xRange / Math.Max(1, estimatedGridSize - 1);
      double avgSpacingY = yRange / Math.Max(1, estimatedGridSize - 1);
      double avgSpacing = Math.Min(avgSpacingX, avgSpacingY);

      // We want the average vector to be about 70% of the grid spacing
      // So: avgMagnitude * optimalScale = 0.7 * avgSpacing
      if (_avgMagnitude > 1e-10)
      {
        _optimalVectorScale = (0.7 * avgSpacing) / _avgMagnitude;
      }
      else
      {
        _optimalVectorScale = 1.0;
      }

      // Clamp to reasonable range
      _optimalVectorScale = Math.Clamp(_optimalVectorScale, 0.001, 1000);
    }

    /// <summary>
    /// Initializes coordinate system parameters based on canvas size and data bounds.
    /// </summary>
    private void InitializeCoordSysParams()
    {
      if (_vectors.Count == 0) return;

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      if (_coordSystemParams._windowWidth < 1 || _coordSystemParams._windowHeight < 1)
        return;

      double xRange = _dataXMax - _dataXMin;
      double yRange = _dataYMax - _dataYMin;

      // Add small margin if range is zero
      if (xRange < 1e-10) { xRange = 2; _dataXMin -= 1; _dataXMax += 1; }
      if (yRange < 1e-10) { yRange = 2; _dataYMin -= 1; _dataYMax += 1; }

      // Add 10% margin around the data
      double marginX = xRange * 0.1;
      double marginY = yRange * 0.1;

      _coordSystemParams._xMin = _dataXMin - marginX;
      _coordSystemParams._xMax = _dataXMax + marginX;
      _coordSystemParams._yMin = _dataYMin - marginY;
      _coordSystemParams._yMax = _dataYMax + marginY;

      xRange = _coordSystemParams._xMax - _coordSystemParams._xMin;
      yRange = _coordSystemParams._yMax - _coordSystemParams._yMin;

      double midPointX = (_coordSystemParams._xMin + _coordSystemParams._xMax) / 2;
      double midPointY = (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2;

      double availableWidth = _coordSystemParams._windowWidth * 0.95;
      double availableHeight = _coordSystemParams._windowHeight * 0.95;

      if (_preserveAspectRatio)
      {
        double scaleX = availableWidth / xRange;
        double scaleY = availableHeight / yRange;
        double uniformScale = Math.Min(scaleX, scaleY);

        _coordSystemParams._scaleX = uniformScale;
        _coordSystemParams._scaleY = uniformScale;
      }
      else
      {
        _coordSystemParams._scaleX = availableWidth / xRange;
        _coordSystemParams._scaleY = availableHeight / yRange;
      }

      _coordSystemParams._centerX = _coordSystemParams._windowWidth / 2 - midPointX * _coordSystemParams._scaleX;
      _coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + midPointY * _coordSystemParams._scaleY;
    }

    /// <summary>
    /// Updates the UI text boxes with current data bounds and info.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtXMin.Text = FormatValue(_dataXMin);
      txtXMax.Text = FormatValue(_dataXMax);
      txtYMin.Text = FormatValue(_dataYMin);
      txtYMax.Text = FormatValue(_dataYMax);
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
      if (Math.Abs(value) >= 10000 || (Math.Abs(value) > 0 && Math.Abs(value) < 0.01))
        return value.ToString("E2");
      if (Math.Abs(value - Math.Round(value)) < 1e-10)
        return ((long)Math.Round(value)).ToString();
      return value.ToString("G4");
    }

    /// <summary>
    /// Redraws the entire visualization.
    /// </summary>
    private void Redraw()
    {
      if (_vectors.Count == 0 || !_isInitialized) return;

      InitializeCoordSysParams();

      if (_coordSystemParams._windowWidth < 1 || _coordSystemParams._windowHeight < 1)
        return;

      mainCanvas.Children.Clear();

      // Draw coordinate system
      DrawCoordSystem();

      // Draw vectors
      DrawVectors();
    }

    /// <summary>
    /// Draws the coordinate system (axes at origin if visible).
    /// </summary>
    private void DrawCoordSystem()
    {
      // Draw X axis if y=0 is in range
      if (_coordSystemParams._yMin <= 0 && _coordSystemParams._yMax >= 0)
      {
        double y0 = _coordSystemParams._centerY;
        Line xAxis = new Line
        {
          X1 = 0,
          Y1 = y0,
          X2 = _coordSystemParams._windowWidth,
          Y2 = y0,
          Stroke = Brushes.LightGray,
          StrokeThickness = 1
        };
        mainCanvas.Children.Add(xAxis);
      }

      // Draw Y axis if x=0 is in range
      if (_coordSystemParams._xMin <= 0 && _coordSystemParams._xMax >= 0)
      {
        double x0 = _coordSystemParams._centerX;
        Line yAxis = new Line
        {
          X1 = x0,
          Y1 = 0,
          X2 = x0,
          Y2 = _coordSystemParams._windowHeight,
          Stroke = Brushes.LightGray,
          StrokeThickness = 1
        };
        mainCanvas.Children.Add(yAxis);
      }
    }

    /// <summary>
    /// Draws all vector arrows.
    /// </summary>
    private void DrawVectors()
    {
      foreach (var vector in _vectors)
      {
        DrawArrow(vector);
      }
    }

    /// <summary>
    /// Draws a single vector arrow with proper scaling.
    /// </summary>
    private void DrawArrow(Vector2Repr vector)
    {
      double magnitude = Math.Sqrt(vector.Vec.X * vector.Vec.X + vector.Vec.Y * vector.Vec.Y);

      // Skip zero vectors
      if (magnitude < 1e-10) return;

      // Calculate screen coordinates for start point
      double x1 = _coordSystemParams._centerX + vector.Pos.X * _coordSystemParams._scaleX;
      double y1 = _coordSystemParams._centerY - vector.Pos.Y * _coordSystemParams._scaleY;

      // Calculate vector components
      double vx = vector.Vec.X;
      double vy = vector.Vec.Y;

      // Apply normalization if requested
      if (_normalizeVectors)
      {
        vx /= magnitude;
        vy /= magnitude;
      }

      // Apply the combined scale: optimal scale * user scale
      double effectiveScale = _optimalVectorScale * _magnitudeScale;

      // Calculate end point in world coordinates, then convert to screen
      double endX = vector.Pos.X + effectiveScale * vx;
      double endY = vector.Pos.Y + effectiveScale * vy;

      double x2 = _coordSystemParams._centerX + endX * _coordSystemParams._scaleX;
      double y2 = _coordSystemParams._centerY - endY * _coordSystemParams._scaleY;

      // Determine color
      Brush brush;
      if (_colorByMagnitude && _maxMagnitude > _minMagnitude)
      {
        double t = (magnitude - _minMagnitude) / (_maxMagnitude - _minMagnitude);
        brush = GetMagnitudeColor(t);
      }
      else
      {
        brush = new SolidColorBrush(_arrowColor);
      }

      // Draw main line
      Line mainLine = new Line
      {
        X1 = x1, Y1 = y1,
        X2 = x2, Y2 = y2,
        Stroke = brush,
        StrokeThickness = 1.5
      };
      mainCanvas.Children.Add(mainLine);

      // Draw arrowhead (in screen coordinates)
      double screenDx = x2 - x1;
      double screenDy = y2 - y1;
      double screenAngle = Math.Atan2(screenDy, screenDx);
      double arrowAngleRad = _arrowAngle * Math.PI / 180;

      double xArrow1 = x2 - _arrowSize * Math.Cos(screenAngle - arrowAngleRad);
      double yArrow1 = y2 - _arrowSize * Math.Sin(screenAngle - arrowAngleRad);
      double xArrow2 = x2 - _arrowSize * Math.Cos(screenAngle + arrowAngleRad);
      double yArrow2 = y2 - _arrowSize * Math.Sin(screenAngle + arrowAngleRad);

      Line arrow1 = new Line
      {
        X1 = x2, Y1 = y2,
        X2 = xArrow1, Y2 = yArrow1,
        Stroke = brush,
        StrokeThickness = 1.5
      };
      mainCanvas.Children.Add(arrow1);

      Line arrow2 = new Line
      {
        X1 = x2, Y1 = y2,
        X2 = xArrow2, Y2 = yArrow2,
        Stroke = brush,
        StrokeThickness = 1.5
      };
      mainCanvas.Children.Add(arrow2);
    }

    /// <summary>
    /// Gets a color based on normalized magnitude (0 to 1).
    /// Uses a blue-cyan-green-yellow-red gradient.
    /// </summary>
    private Brush GetMagnitudeColor(double t)
    {
      t = Math.Clamp(t, 0, 1);

      byte r, g, b;

      if (t < 0.25)
      {
        // Blue to Cyan
        double localT = t / 0.25;
        r = 0;
        g = (byte)(255 * localT);
        b = 255;
      }
      else if (t < 0.5)
      {
        // Cyan to Green
        double localT = (t - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = (byte)(255 * (1 - localT));
      }
      else if (t < 0.75)
      {
        // Green to Yellow
        double localT = (t - 0.5) / 0.25;
        r = (byte)(255 * localT);
        g = 255;
        b = 0;
      }
      else
      {
        // Yellow to Red
        double localT = (t - 0.75) / 0.25;
        r = 255;
        g = (byte)(255 * (1 - localT));
        b = 0;
      }

      return new SolidColorBrush(Color.FromRgb(r, g, b));
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

        if (type != "VECTOR_FIELD_2D_CARTESIAN")
        {
          MessageBox.Show($"Unsupported format: {type}");
          return false;
        }

        _title = lines[1].Trim();

        for (int i = 2; i < lines.Length; i++)
        {
          if (string.IsNullOrWhiteSpace(lines[i])) continue;

          string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 4) continue;

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);
          double vx = double.Parse(parts[2], CultureInfo.InvariantCulture);
          double vy = double.Parse(parts[3], CultureInfo.InvariantCulture);

          _vectors.Add(new Vector2Repr(
            new Vector2Cartesian(x, y),
            new Vector2Cartesian(vx, vy)));
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
          "Black" => Colors.Black,
          "Blue" => Colors.Blue,
          "Red" => Colors.Red,
          "Green" => Colors.Green,
          "Orange" => Colors.Orange,
          "Purple" => Colors.Purple,
          _ => Colors.Black
        };
      }
      return Colors.Black;
    }

    #region Event Handlers

    /// <summary>
    /// Handles window resize events.
    /// </summary>
    private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles changes to the editable title text box.
    /// </summary>
    private void OnTitleTextChanged(object sender, TextChangedEventArgs e)
    {
      if (_isUpdatingTitle) return;

      _title = txtEditableTitle.Text;
      txtTitle.Text = _title;
    }

    /// <summary>
    /// Handles the preserve aspect ratio checkbox change.
    /// </summary>
    private void OnPreserveAspectRatioChanged(object sender, RoutedEventArgs e)
    {
      _preserveAspectRatio = chkPreserveAspectRatio.IsChecked ?? false;
      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles the normalize vectors checkbox change.
    /// </summary>
    private void OnNormalizeVectorsChanged(object sender, RoutedEventArgs e)
    {
      _normalizeVectors = chkNormalizeVectors.IsChecked ?? false;
      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles the color by magnitude checkbox change.
    /// </summary>
    private void OnColorByMagnitudeChanged(object sender, RoutedEventArgs e)
    {
      _colorByMagnitude = chkColorByMagnitude.IsChecked ?? true;
      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles arrow color selection change.
    /// </summary>
    private void cmbArrowColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if (cmbArrowColor == null) return;

      _arrowColor = GetColorFromComboBox(cmbArrowColor);
      if (_vectors.Count > 0 && !_colorByMagnitude && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles magnitude scale slider change.
    /// </summary>
    private void sliderMagnitudeScale_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if (sliderMagnitudeScale == null || txtMagnitudeScale == null) return;

      _magnitudeScale = sliderMagnitudeScale.Value;
      txtMagnitudeScale.Text = _magnitudeScale.ToString("F2");

      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    /// <summary>
    /// Handles arrow size slider change.
    /// </summary>
    private void sliderArrowSize_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if (sliderArrowSize == null || txtArrowSizeDisplay == null) return;

      _arrowSize = sliderArrowSize.Value;
      txtArrowSizeDisplay.Text = ((int)_arrowSize).ToString();

      if (_vectors.Count > 0 && _isInitialized)
        Redraw();
    }

    #endregion
  }
}