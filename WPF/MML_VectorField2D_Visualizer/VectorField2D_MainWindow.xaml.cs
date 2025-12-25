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
  ///   <item><description>Vector arrows with customizable scale and size</description></item>
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

    // Data bounds
    private double _dataXMin, _dataXMax, _dataYMin, _dataYMax;
    private double _minMagnitude, _maxMagnitude;

    // Display options
    private bool _preserveAspectRatio = false;
    private bool _normalizeVectors = false;
    private bool _colorByMagnitude = true;
    private double _vectorScale = 0.5;
    private double _arrowSize = 10;
    private double _arrowAngle = 30;
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
      InitializeCoordSysParams();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      Redraw();
    }

    /// <summary>
    /// Calculates data bounds and magnitude range from loaded vectors.
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

      foreach (var v in _vectors)
      {
        _dataXMin = Math.Min(_dataXMin, v.Pos.X);
        _dataXMax = Math.Max(_dataXMax, v.Pos.X);
        _dataYMin = Math.Min(_dataYMin, v.Pos.Y);
        _dataYMax = Math.Max(_dataYMax, v.Pos.Y);

        double magnitude = Math.Sqrt(v.Vec.X * v.Vec.X + v.Vec.Y * v.Vec.Y);
        _minMagnitude = Math.Min(_minMagnitude, magnitude);
        _maxMagnitude = Math.Max(_maxMagnitude, magnitude);
      }

      _coordSystemParams._numPoints = _vectors.Count;
    }

    /// <summary>
    /// Initializes coordinate system parameters based on canvas size and data bounds.
    /// </summary>
    private void InitializeCoordSysParams()
    {
      if (_vectors.Count == 0) return;

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      double xRange = _dataXMax - _dataXMin;
      double yRange = _dataYMax - _dataYMin;

      // Add small margin if range is zero
      if (xRange < 1e-10) { xRange = 1; _dataXMin -= 0.5; _dataXMax += 0.5; }
      if (yRange < 1e-10) { yRange = 1; _dataYMin -= 0.5; _dataYMax += 0.5; }

      _coordSystemParams._xMin = _dataXMin;
      _coordSystemParams._xMax = _dataXMax;
      _coordSystemParams._yMin = _dataYMin;
      _coordSystemParams._yMax = _dataYMax;

      double midPointX = (_dataXMin + _dataXMax) / 2;
      double midPointY = (_dataYMin + _dataYMax) / 2;

      double availableWidth = _coordSystemParams._windowWidth * 0.9;
      double availableHeight = _coordSystemParams._windowHeight * 0.9;

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
      return value.ToString("G6");
    }

    /// <summary>
    /// Redraws the entire visualization.
    /// </summary>
    private void Redraw()
    {
      if (_vectors.Count == 0) return;

      InitializeCoordSysParams();
      mainCanvas.Children.Clear();

      // Draw coordinate system
      DrawCoordSystem();

      // Draw vectors
      DrawVectors();
    }

    /// <summary>
    /// Draws the coordinate system (axes).
    /// </summary>
    private void DrawCoordSystem()
    {
      // Draw X axis if y=0 is in range
      if (_dataYMin <= 0 && _dataYMax >= 0)
      {
        double y0 = _coordSystemParams._centerY;
        Line xAxis = new Line
        {
          X1 = 0,
          Y1 = y0,
          X2 = _coordSystemParams._windowWidth,
          Y2 = y0,
          Stroke = Brushes.Gray,
          StrokeThickness = 1,
          StrokeDashArray = new DoubleCollection { 4, 2 }
        };
        mainCanvas.Children.Add(xAxis);
      }

      // Draw Y axis if x=0 is in range
      if (_dataXMin <= 0 && _dataXMax >= 0)
      {
        double x0 = _coordSystemParams._centerX;
        Line yAxis = new Line
        {
          X1 = x0,
          Y1 = 0,
          X2 = x0,
          Y2 = _coordSystemParams._windowHeight,
          Stroke = Brushes.Gray,
          StrokeThickness = 1,
          StrokeDashArray = new DoubleCollection { 4, 2 }
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
    /// Draws a single vector arrow.
    /// </summary>
    private void DrawArrow(Vector2Repr vector)
    {
      double magnitude = Math.Sqrt(vector.Vec.X * vector.Vec.X + vector.Vec.Y * vector.Vec.Y);

      // Calculate screen coordinates for start point
      double x1 = _coordSystemParams._centerX + vector.Pos.X * _coordSystemParams._scaleX;
      double y1 = _coordSystemParams._centerY - vector.Pos.Y * _coordSystemParams._scaleY;

      // Calculate vector components (optionally normalized)
      double vx = vector.Vec.X;
      double vy = vector.Vec.Y;

      if (_normalizeVectors && magnitude > 1e-10)
      {
        vx /= magnitude;
        vy /= magnitude;
      }

      // Calculate screen coordinates for end point
      double x2 = _coordSystemParams._centerX + (vector.Pos.X + _vectorScale * vx) * _coordSystemParams._scaleX;
      double y2 = _coordSystemParams._centerY - (vector.Pos.Y + _vectorScale * vy) * _coordSystemParams._scaleY;

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

      // Draw arrowhead
      double angle = Math.Atan2(vy, vx);
      double arrowAngleRad = _arrowAngle * Math.PI / 180;

      double xArrow1 = x2 - _arrowSize * Math.Cos(angle + arrowAngleRad);
      double yArrow1 = y2 + _arrowSize * Math.Sin(angle + arrowAngleRad);
      double xArrow2 = x2 - _arrowSize * Math.Cos(angle - arrowAngleRad);
      double yArrow2 = y2 + _arrowSize * Math.Sin(angle - arrowAngleRad);

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
      if (_vectors.Count > 0)
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
      if (_vectors.Count > 0)
        Redraw();
    }

    /// <summary>
    /// Handles the normalize vectors checkbox change.
    /// </summary>
    private void OnNormalizeVectorsChanged(object sender, RoutedEventArgs e)
    {
      _normalizeVectors = chkNormalizeVectors.IsChecked ?? false;
      if (_vectors.Count > 0)
        Redraw();
    }

    /// <summary>
    /// Handles the color by magnitude checkbox change.
    /// </summary>
    private void OnColorByMagnitudeChanged(object sender, RoutedEventArgs e)
    {
      _colorByMagnitude = chkColorByMagnitude.IsChecked ?? true;
      if (_vectors.Count > 0)
        Redraw();
    }

    /// <summary>
    /// Handles arrow color selection change.
    /// </summary>
    private void cmbArrowColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if (cmbArrowColor == null) return;

      _arrowColor = GetColorFromComboBox(cmbArrowColor);
      if (_vectors.Count > 0 && !_colorByMagnitude)
        Redraw();
    }

    /// <summary>
    /// Handles vector scale text change.
    /// </summary>
    private void OnVectorScaleChanged(object sender, TextChangedEventArgs e)
    {
      if (txtVectorScale == null) return;

      if (double.TryParse(txtVectorScale.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out double scale) && scale > 0)
      {
        _vectorScale = scale;
        if (_vectors.Count > 0)
          Redraw();
      }
    }

    /// <summary>
    /// Handles arrow size text change.
    /// </summary>
    private void OnArrowSizeChanged(object sender, TextChangedEventArgs e)
    {
      if (txtArrowSize == null) return;

      if (double.TryParse(txtArrowSize.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out double size) && size > 0)
      {
        _arrowSize = size;
        if (_vectors.Count > 0)
          Redraw();
      }
    }

    #endregion
  }
}