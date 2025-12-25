using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Media3D;

using MML;
using WPF3DHelperLib;
using MML_VisualizersBase;

namespace MML_ParametricCurve3D_Visualizer
{
  /// <summary>
  /// Main window for the 3D Parametric Curve Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides interactive 3D visualization of parametric curves in space.
  /// It supports loading curve data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Display of single or multiple 3D parametric curves</description></item>
  ///   <item><description>Interactive camera control (rotate, pan, zoom)</description></item>
  ///   <item><description>Animation of points along curve trajectories with pause/resume/reset</description></item>
  ///   <item><description>Adjustable animation speed and line width</description></item>
  ///   <item><description>Interactive legend with visibility toggles for each curve</description></item>
  ///   <item><description>Editable graph title</description></item>
  /// </list>
  /// <para>
  /// The application expects data files to be passed as command-line arguments.
  /// Each file should contain parametric curve data in the PARAMETRIC_CURVE_CARTESIAN_3D format.
  /// </para>
  /// </remarks>
  public partial class MainWindow : Window
  {
    private readonly List<SolidColorBrush> _brushes = Defaults.GetBrushList();

    private double _axisLen = 500;
    private double _lineWidth = 0.25;

    private readonly List<LoadedCurve> _curves = new List<LoadedCurve>();
    private readonly List<Sphere> _spheres = new List<Sphere>();

    private readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();
    private readonly Model3DGroup _myModel3DGroup = new Model3DGroup();

    private string _title = "";
    private bool _isUpdatingTitle = false;
    private bool _sceneInitialized = false;

    // Animation state
    private bool _isAnimating = false;
    private bool _isPaused = false;
    private int _currentAnimationStep = 0;
    private CancellationTokenSource? _animationCancellation;

    // Store parameter bounds across all curves
    private double _dataTMin, _dataTMax;
    private int _totalNumPoints;

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

      int numInputs = args.Length - 1;
      for (int i = 0; i < numInputs; i++)
      {
        var fileName = args[i + 1];
        var loadedCurve = LoadData(fileName);
        if (loadedCurve != null)
        {
          _curves.Add(loadedCurve);
          _spheres.Add(new Sphere());
        }
      }

      if (_curves.Count > 0)
      {
        _title = _curves[0].Title;
        CalculateDataBounds();
      }

      InitScene();
      UpdateLegend();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      // Subscribe to legend visibility changes
      LegendWidgetControl.ItemVisibilityChanged += OnLegendItemVisibilityChanged;
    }

    /// <summary>
    /// Calculates the overall data bounds from all loaded curves.
    /// </summary>
    private void CalculateDataBounds()
    {
      if (_curves.Count == 0) return;

      _dataTMin = _curves[0].GetMinT();
      _dataTMax = _curves[0].GetMaxT();
      _totalNumPoints = _curves[0].GetNumPoints();

      for (int i = 1; i < _curves.Count; i++)
      {
        _dataTMin = Math.Min(_dataTMin, _curves[i].GetMinT());
        _dataTMax = Math.Max(_dataTMax, _curves[i].GetMaxT());
        _totalNumPoints = Math.Max(_totalNumPoints, _curves[i].GetNumPoints());
      }
    }

    /// <summary>
    /// Updates the UI text boxes with current curve information.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtNumPoints.Text = _totalNumPoints.ToString();
      txtTMin.Text = FormatValue(_dataTMin);
      txtTMax.Text = FormatValue(_dataTMax);
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
    /// Initializes the 3D scene with camera, lights, coordinate system, and curves.
    /// </summary>
    /// <remarks>
    /// This is called once at startup to set up the camera and viewport.
    /// For subsequent redraws (e.g., visibility changes), use <see cref="RedrawScene"/>.
    /// </remarks>
    private void InitScene()
    {
      _myModel3DGroup.Children.Clear();

      // Only initialize camera on first call
      if (!_sceneInitialized)
      {
        _helper.InitCamera(new Point3D(350, 100, 350));
        _sceneInitialized = true;
      }

      // Setup lighting
      SetupLighting();

      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };
      myViewport3D.Children.Clear();
      myViewport3D.Children.Add(myModelVisual3D);

      // Draw coordinate axes
      Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 1, _axisLen);

      // Draw all visible curves
      DrawCurves();
    }

    /// <summary>
    /// Redraws the scene without resetting camera position.
    /// </summary>
    /// <remarks>
    /// Use this method when visibility changes or line width is adjusted,
    /// to preserve the current camera view.
    /// </remarks>
    private void RedrawScene()
    {
      _myModel3DGroup.Children.Clear();

      // Setup lighting
      SetupLighting();

      // Draw coordinate axes
      Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 1, _axisLen);

      // Draw all visible curves
      DrawCurves();

      myViewport3D.InvalidateVisual();
    }

    /// <summary>
    /// Sets up the scene lighting.
    /// </summary>
    private void SetupLighting()
    {
      AmbientLight ambLight = new AmbientLight { Color = Colors.White };
      _myModel3DGroup.Children.Add(ambLight);

      DirectionalLight myDirectionalLight1 = new DirectionalLight(
        diffuseColor: Colors.White, direction: new Vector3D(-0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight1);

      DirectionalLight myDirectionalLight2 = new DirectionalLight(
        diffuseColor: Colors.White, direction: new Vector3D(0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight2);
    }

    /// <summary>
    /// Draws all visible curves to the scene.
    /// </summary>
    private void DrawCurves()
    {
      for (int i = 0; i < _curves.Count; i++)
      {
        if (!_curves[i].IsVisible)
        {
          _curves[i].CurveModel = null;
          continue;
        }

        MeshGeometry3D line = Geometries.CreatePolyLine(_curves[i].CurveTrace, _lineWidth, 10);
        DiffuseMaterial lineMaterial = new DiffuseMaterial(_brushes[i % _brushes.Count]);
        GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        _curves[i].CurveModel = lineModel;
        _myModel3DGroup.Children.Add(lineModel);
      }
    }

    /// <summary>
    /// Loads parametric curve data from a file.
    /// </summary>
    /// <param name="inFileName">The path to the data file.</param>
    /// <returns>The loaded curve, or null if loading fails.</returns>
    private LoadedCurve? LoadData(string inFileName)
    {
      if (!File.Exists(inFileName))
      {
        MessageBox.Show($"File does not exist: {inFileName}");
        return null;
      }

      try
      {
        string[] lines = File.ReadAllLines(inFileName);
        if (lines.Length == 0)
        {
          MessageBox.Show($"File is empty: {inFileName}");
          return null;
        }

        string type = lines[0].Trim();

        if (type != "PARAMETRIC_CURVE_CARTESIAN_3D")
        {
          MessageBox.Show($"Unknown file type: {type}");
          return null;
        }

        LoadedCurve curve = new LoadedCurve { Title = lines[1] };

        for (int i = 5; i < lines.Length; i++)
        {
          if (string.IsNullOrWhiteSpace(lines[i])) continue;

          string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 4) continue;

          double t = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[2], CultureInfo.InvariantCulture);
          double z = double.Parse(parts[3], CultureInfo.InvariantCulture);

          curve.AddPoint(t, x, y, z);
        }

        return curve;
      }
      catch (Exception e)
      {
        MessageBox.Show($"Error loading file {inFileName}\nMessage: {e.Message}");
        return null;
      }
    }

    /// <summary>
    /// Populates the legend widget with entries for all loaded curves.
    /// </summary>
    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();

      for (int i = 0; i < _curves.Count; i++)
      {
        LegendWidgetControl.LegendItems.Add(new LegendItem
        {
          Title = _curves[i].Title,
          Color = _brushes[i % _brushes.Count],
          IsVisible = true
        });
      }
    }

    /// <summary>
    /// Handles legend item visibility changes to show/hide curves.
    /// </summary>
    private void OnLegendItemVisibilityChanged(object? sender, EventArgs e)
    {
      UpdateCurveVisibility();
      RedrawScene();
    }

    /// <summary>
    /// Synchronizes curve visibility with legend checkbox states.
    /// </summary>
    private void UpdateCurveVisibility()
    {
      for (int i = 0; i < _curves.Count && i < LegendWidgetControl.LegendItems.Count; i++)
      {
        _curves[i].IsVisible = LegendWidgetControl.LegendItems[i].IsVisible;
      }
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

    public void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);
    }

    private void Window_KeyDown(object sender, KeyEventArgs e)
    {
      _helper.Window_KeyDown(myViewport3D, e);
    }

    #endregion

    #region Animation Controls

    /// <summary>
    /// Creates animated sphere markers for each visible curve.
    /// </summary>
    private void CreateAnimationSpheres()
    {
      for (int i = 0; i < _curves.Count; i++)
      {
        if (!_curves[i].IsVisible) continue;

        MeshGeometry3D sphereGeometry = (i == 0)
          ? Geometries.CreateSphere(new Point3D(0, 0, 0), 5)
          : Geometries.CreateSphere(new Point3D(0, 0, 0), 2);

        TranslateTransform3D offset = new TranslateTransform3D
        {
          OffsetX = _spheres[i].X,
          OffsetY = _spheres[i].Y,
          OffsetZ = _spheres[i].Z
        };

        var sphereMaterial = new DiffuseMaterial(_brushes[i % _brushes.Count]);
        GeometryModel3D sphereModel = new GeometryModel3D(sphereGeometry, sphereMaterial)
        {
          Transform = offset
        };

        _spheres[i].RefGeomModel = sphereModel;
        _myModel3DGroup.Children.Add(sphereModel);
      }
    }

    /// <summary>
    /// Updates sphere positions to the specified animation step.
    /// </summary>
    /// <param name="step">The animation step (index into curve trace).</param>
    private void UpdateSpherePositions(int step)
    {
      for (int i = 0; i < _curves.Count; i++)
      {
        if (!_curves[i].IsVisible || step >= _curves[i].CurveTrace.Count) continue;

        if (_spheres[i]?.RefGeomModel != null)
        {
          TranslateTransform3D offset = new TranslateTransform3D
          {
            OffsetX = _curves[i].CurveTrace[step].X,
            OffsetY = _curves[i].CurveTrace[step].Y,
            OffsetZ = _curves[i].CurveTrace[step].Z
          };
          _spheres[i].RefGeomModel.Transform = offset;
        }
      }
    }

    /// <summary>
    /// Gets the current animation delay in milliseconds based on the speed TextBox.
    /// </summary>
    /// <returns>Delay in milliseconds.</returns>
    private int GetAnimationDelayMs()
    {
      int delayMs = 100; // Default: 10 points per second

      this.Dispatcher.Invoke(() =>
      {
        if (double.TryParse(txtAnimationSpeed.Text, out double pointsPerSecond) && pointsPerSecond > 0)
        {
          delayMs = (int)(1000.0 / pointsPerSecond);
        }
      });

      return delayMs;
    }

    /// <summary>
    /// Handles the Start Animation button click.
    /// </summary>
    private void btnStartAnimation_Click(object sender, RoutedEventArgs e)
    {
      // If already animating but paused, resume
      if (_isAnimating && _isPaused)
      {
        _isPaused = false;
        btnPauseAnimation.Content = "Pause";
        return;
      }

      // If already animating, stop first
      if (_isAnimating)
      {
        StopAnimation();
      }

      // Validate speed input
      if (!double.TryParse(txtAnimationSpeed.Text, out double pointsPerSecond) || pointsPerSecond <= 0)
      {
        txtAnimationSpeed.Text = "10";
      }

      RedrawScene();
      CreateAnimationSpheres();

      // Position spheres at current step
      UpdateSpherePositions(_currentAnimationStep);

      int numSteps = _curves.Count > 0 ? _curves[0].CurveTrace.Count : 0;

      _isAnimating = true;
      _isPaused = false;
      _animationCancellation = new CancellationTokenSource();

      Task.Run(() => Animate(numSteps, _animationCancellation.Token));
    }

    /// <summary>
    /// Handles the Pause button click.
    /// </summary>
    private void btnPauseAnimation_Click(object sender, RoutedEventArgs e)
    {
      if (!_isAnimating) return;

      _isPaused = !_isPaused;
      btnPauseAnimation.Content = _isPaused ? "Resume" : "Pause";
    }

    /// <summary>
    /// Handles the Reset button click.
    /// </summary>
    private void btnResetAnimation_Click(object sender, RoutedEventArgs e)
    {
      // Stop any running animation
      StopAnimation();

      // Reset to beginning
      _currentAnimationStep = 0;
      _isPaused = false;
      btnPauseAnimation.Content = "Pause";

      // Redraw scene and create spheres at starting position
      RedrawScene();
      CreateAnimationSpheres();
      UpdateSpherePositions(0);
    }

    /// <summary>
    /// Stops the running animation.
    /// </summary>
    private void StopAnimation()
    {
      _animationCancellation?.Cancel();
      _isAnimating = false;
      _isPaused = false;
    }

    /// <summary>
    /// Animates spheres along their respective curve trajectories.
    /// </summary>
    /// <param name="numSteps">The total number of animation steps.</param>
    /// <param name="cancellationToken">Token to cancel the animation.</param>
    /// <remarks>
    /// The animation speed is read dynamically from the UI, allowing speed changes
    /// while the animation is running or paused.
    /// </remarks>
    private void Animate(int numSteps, CancellationToken cancellationToken)
    {
      while (_currentAnimationStep < numSteps && !cancellationToken.IsCancellationRequested)
      {
        // Handle pause
        while (_isPaused && !cancellationToken.IsCancellationRequested)
        {
          Thread.Sleep(50);
        }

        if (cancellationToken.IsCancellationRequested)
          break;

        this.Dispatcher.Invoke(() => UpdateSpherePositions(_currentAnimationStep));

        _currentAnimationStep++;

        // Read delay dynamically to allow speed changes during animation
        int delayMs = GetAnimationDelayMs();
        Thread.Sleep(delayMs);
      }

      // Animation completed
      if (!cancellationToken.IsCancellationRequested)
      {
        this.Dispatcher.Invoke(() =>
        {
          _isAnimating = false;
          _currentAnimationStep = 0;
        });
      }
    }

    #endregion

    #region Button Click Handlers

    /// <summary>
    /// Handles the increase line width button click.
    /// </summary>
    private void btnIncWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 1.1;
      RedrawScene();
    }

    /// <summary>
    /// Handles the decrease line width button click.
    /// </summary>
    private void btnDecWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 0.9;
      RedrawScene();
    }

    #endregion
  }
}
