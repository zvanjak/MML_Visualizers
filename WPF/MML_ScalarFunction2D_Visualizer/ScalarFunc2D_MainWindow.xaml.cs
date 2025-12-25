using System;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Media3D;

using MML;
using WPF3DHelperLib;
using Matrix = MML.Matrix;

namespace MML_ScalarFunction2Visualizer
{
  /// <summary>
  /// Main window for the Scalar Function 2D Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides interactive 3D visualization of scalar functions f(x,y).
  /// It renders a surface plot in 3D space with features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>3D surface rendering with customizable color</description></item>
  ///   <item><description>Optional surface point markers</description></item>
  ///   <item><description>Interactive camera control (rotate, pan, zoom)</description></item>
  ///   <item><description>Editable graph title</description></item>
  ///   <item><description>Display of X, Y, Z bounds and grid information</description></item>
  /// </list>
  /// </remarks>
  public partial class MainWindow : Window
  {
    private Matrix? _vals;

    private double _xMin, _xMax;
    private double _yMin, _yMax;
    private double _zMin, _zMax;
    private int _numPointsX, _numPointsY;

    private string _title = "";
    private bool _isUpdatingTitle = false;
    private bool _sceneInitialized = false;

    private double _scaleX = 10;
    private double _scaleY = 10;
    private bool _showSurfacePoints = true;

    private Color _surfaceColor = Colors.Red;
    private Color _pointsColor = Colors.Blue;

    private readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();
    private Model3DGroup? _myModel3DGroup;

    /// <summary>
    /// Initializes a new instance of the <see cref="MainWindow"/> class.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }

      var fileName = args[1];

      if (LoadData(fileName))
      {
        InitializeScene();
        UpdateUITextBoxes();

        txtTitle.Text = _title;
        _isUpdatingTitle = true;
        txtEditableTitle.Text = _title;
        _isUpdatingTitle = false;

        chkShowSurfacePoints.IsChecked = _showSurfacePoints;
      }
    }

    /// <summary>
    /// Updates the UI text boxes with current data bounds and grid info.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtXMin.Text = FormatValue(_xMin);
      txtXMax.Text = FormatValue(_xMax);
      txtYMin.Text = FormatValue(_yMin);
      txtYMax.Text = FormatValue(_yMax);
      txtZMin.Text = FormatValue(_zMin);
      txtZMax.Text = FormatValue(_zMax);
      txtNumPointsX.Text = _numPointsX.ToString();
      txtNumPointsY.Text = _numPointsY.ToString();
      txtTotalPoints.Text = (_numPointsX * _numPointsY).ToString();
    }

    /// <summary>
    /// Formats a numeric value for display.
    /// </summary>
    private string FormatValue(double value)
    {
      if (Math.Abs(value) >= 10000 || (Math.Abs(value) > 0 && Math.Abs(value) < 0.01))
        return value.ToString("E2");
      if (Math.Abs(value - Math.Round(value)) < 1e-10)
        return ((long)Math.Round(value)).ToString();
      return value.ToString("G6");
    }

    /// <summary>
    /// Clears the 3D scene.
    /// </summary>
    private void ClearScene()
    {
      myViewport3D.Children.Clear();
    }

    /// <summary>
    /// Initializes the 3D scene with camera, lights, coordinate system, and surface.
    /// </summary>
    private void InitializeScene()
    {
      ClearScene();

      _myModel3DGroup = new Model3DGroup();

      // Only initialize camera on first call
      if (!_sceneInitialized)
      {
        _helper.InitCamera(new Point3D(180, 80, 150));
        _sceneInitialized = true;
      }

      _helper.InitLights(_myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };
      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(_myModel3DGroup, 0.5, 500);

      if (_vals == null) return;

      // Draw surface points if enabled
      if (_showSurfacePoints)
      {
        DrawSurfacePoints();
      }

      // Draw surface
      DrawSurface();
    }

    /// <summary>
    /// Redraws the scene without resetting camera position.
    /// </summary>
    private void RedrawScene()
    {
      if (_vals == null) return;

      ClearScene();

      _myModel3DGroup = new Model3DGroup();
      _helper.InitLights(_myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };
      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(_myModel3DGroup, 0.5, 500);

      if (_showSurfacePoints)
      {
        DrawSurfacePoints();
      }

      DrawSurface();
    }

    /// <summary>
    /// Draws the surface point markers.
    /// </summary>
    private void DrawSurfacePoints()
    {
      if (_vals == null || _myModel3DGroup == null) return;

      var pointMaterial = new DiffuseMaterial(new SolidColorBrush(_pointsColor));

      for (int i = 0; i < _vals.Rows; i++)
      {
        for (int j = 0; j < _vals.Cols; j++)
        {
          double x = _scaleX * (_xMin + i * (_xMax - _xMin) / _numPointsX);
          double y = _scaleY * (_yMin + j * (_yMax - _yMin) / _numPointsY);
          double z = _vals.ElemAt(i, j);

          MeshGeometry3D cube = Geometries.CreateCube(new Point3D(x, y, z), 0.75);
          GeometryModel3D cubeModel = new GeometryModel3D(cube, pointMaterial);

          _myModel3DGroup.Children.Add(cubeModel);
        }
      }
    }

    /// <summary>
    /// Draws the 3D surface.
    /// </summary>
    private void DrawSurface()
    {
      if (_vals == null || _myModel3DGroup == null) return;

      MeshGeometry3D surface = Geometries.CreateSurface(_vals, _xMin, _xMax, _yMin, _yMax, _scaleX, _scaleY);
      var surfaceMaterial = new DiffuseMaterial(new SolidColorBrush(_surfaceColor));
      GeometryModel3D surfaceModel = new GeometryModel3D(surface, surfaceMaterial);

      _myModel3DGroup.Children.Add(surfaceModel);
    }

    /// <summary>
    /// Loads scalar function data from a file.
    /// </summary>
    /// <param name="inFileName">The path to the data file.</param>
    /// <returns>True if loading was successful, false otherwise.</returns>
    private bool LoadData(string inFileName)
    {
      if (!File.Exists(inFileName))
      {
        MessageBox.Show($"File does not exist: {inFileName}");
        return false;
      }

      try
      {
        string[] lines = File.ReadAllLines(inFileName);
        string type = lines[0].Trim();

        if (type != "SCALAR_FUNCTION_CARTESIAN_2D")
        {
          MessageBox.Show($"Unsupported format: {type}");
          return false;
        }

        _title = lines[1];

        string[] partsX1 = lines[2].Split(' ');
        _xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

        string[] partsX2 = lines[3].Split(' ');
        _xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

        string[] partsNumPointsX = lines[4].Split(' ');
        _numPointsX = int.Parse(partsNumPointsX[1]);

        string[] partsY1 = lines[5].Split(' ');
        _yMin = double.Parse(partsY1[1], CultureInfo.InvariantCulture);

        string[] partsY2 = lines[6].Split(' ');
        _yMax = double.Parse(partsY2[1], CultureInfo.InvariantCulture);

        string[] partsNumPointsY = lines[7].Split(' ');
        _numPointsY = int.Parse(partsNumPointsY[1]);

        _vals = new Matrix(_numPointsX, _numPointsY);

        _zMin = double.MaxValue;
        _zMax = double.MinValue;

        int count = 0;
        for (int i = 8; i < lines.Length; i++)
        {
          if (string.IsNullOrWhiteSpace(lines[i])) continue;

          string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 3) continue;

          double z = double.Parse(parts[2], CultureInfo.InvariantCulture);

          _vals.SetElemAt(count / _numPointsY, count % _numPointsY, z);

          // Track Z bounds
          _zMin = Math.Min(_zMin, z);
          _zMax = Math.Max(_zMax, z);

          count++;
        }

        if (count != _numPointsX * _numPointsY)
        {
          MessageBox.Show($"Count does not match expected points.\nCount = {count}, Expected = {_numPointsX * _numPointsY}");
          return false;
        }

        return true;
      }
      catch (Exception e)
      {
        MessageBox.Show($"Error loading file {inFileName}\nMessage: {e.Message}");
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
          "Red" => Colors.Red,
          "Blue" => Colors.Blue,
          "Green" => Colors.Green,
          "Orange" => Colors.Orange,
          "Purple" => Colors.Purple,
          "Cyan" => Colors.Cyan,
          "Yellow" => Colors.Yellow,
          "Gray" => Colors.Gray,
          "Black" => Colors.Black,
          "White" => Colors.White,
          _ => Colors.Red
        };
      }
      return Colors.Red;
    }

    #region Event Handlers

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
    /// Handles the show surface points checkbox click.
    /// </summary>
    private void chkShowSurfacePoints_Click(object sender, RoutedEventArgs e)
    {
      _showSurfacePoints = chkShowSurfacePoints.IsChecked ?? false;
      RedrawScene();
    }

    /// <summary>
    /// Handles surface color selection change.
    /// </summary>
    private void cmbSurfaceColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if (!_sceneInitialized) return;

      _surfaceColor = GetColorFromComboBox(cmbSurfaceColor);
      RedrawScene();
    }

    /// <summary>
    /// Handles points color selection change.
    /// </summary>
    private void cmbPointsColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if (!_sceneInitialized) return;

      _pointsColor = GetColorFromComboBox(cmbPointsColor);
      RedrawScene();
    }

    #endregion

    #region Mouse and Keyboard Event Handlers

    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonDown(e.GetPosition(this));
    }

    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonUp();
    }

    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonDown(e.GetPosition(this));
    }

    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonUp();
    }

    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      _helper.Window_MouseMove(myViewport3D, e.GetPosition(this), sender, e);
    }

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);
    }

    private void Window_KeyDown(object sender, KeyEventArgs e)
    {
      _helper.Window_KeyDown(myViewport3D, e);
    }

    #endregion
  }
}
