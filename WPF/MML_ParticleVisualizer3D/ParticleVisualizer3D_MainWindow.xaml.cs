using MML;
using MML_VisualizersBase;
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
using WPF3DHelperLib;

namespace MML_ParticleVisualizer3D
{
  /// <summary>
  /// Display mode options for the 3D visualization.
  /// </summary>
  public enum DisplayMode
  {
    /// <summary>Only coordinate axes are shown.</summary>
    None,
    /// <summary>Bounding box with planes in the positive quadrant only.</summary>
    BoundingBox,
    /// <summary>Full coordinate planes extending in both directions from origin.</summary>
    CoordinatePlanes
  }

  /// <summary>
  /// Main window for the 3D Particle Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides 3D visualization of particle simulations.
  /// It supports loading simulation data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>3D playback of particle simulation trajectories</description></item>
  ///   <item><description>Start, Pause, and Reset controls with progress bar</description></item>
  ///   <item><description>Adjustable animation speed and refresh rate</description></item>
  ///   <item><description>Interactive camera control (rotate, pan, zoom, keyboard)</description></item>
  ///   <item><description>Display mode: None, Bounding box, or Coordinate planes</description></item>
  ///   <item><description>Legend showing particle names and colors</description></item>
  ///   <item><description>Editable simulation title</description></item>
  /// </list>
  /// </remarks>
  public partial class ParticleVisualizer3D_MainWindow : Window
  {
    private readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    private double _axisLen = 500;
    private double _boxLen = 1000;
    private double _lineWidth = 0.25;

    private double _containerWidth = 1000;
    private double _containerHeight = 1000;
    private double _containerDepth = 1000;

    private DisplayMode _displayMode = DisplayMode.BoundingBox;
    private bool _sceneInitialized = false;

    private int _numSteps = 0;
    private int _stepDelayMs = 10;
    private int _refreshEvery = 1;

    private int _currStep = 0;
    private bool _isPaused = false;
    private bool _isRunning = false;

    private string _title = "Particle Simulation 3D";
    private bool _isUpdatingTitle = false;

    private readonly List<ParticleData3D> _balls = new List<ParticleData3D>();
    private Model3DGroup _myModel3DGroup = new Model3DGroup();

    /// <summary>
    /// Initializes a new instance of the <see cref="ParticleVisualizer3D_MainWindow"/> class.
    /// </summary>
    public ParticleVisualizer3D_MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }

      var fileName = args[1];

      if (!LoadData(fileName))
      {
        MessageBox.Show($"Error loading data from file {fileName}");
        return;
      }

      _title = $"Particle Simulation 3D - {System.IO.Path.GetFileNameWithoutExtension(fileName)}";

      InitializeScene();
      UpdateLegend();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      // Set initial display mode radio button
      rbDisplayBoundingBox.IsChecked = true;

      // Set initial button states
      btnPauseSim.IsEnabled = false;
      btnRestartSim.IsEnabled = false;
    }

    /// <summary>
    /// Updates the legend with particle information.
    /// </summary>
    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();

      for (int i = 0; i < _balls.Count && i < 10; i++)
      {
        LegendWidgetControl.LegendItems.Add(new LegendItem
        {
          Title = _balls[i].Name,
          Color = new BrushConverter().ConvertFromString(_balls[i].Color) as SolidColorBrush,
          IsVisible = true
        });
      }
    }

    /// <summary>
    /// Updates the UI text boxes with current simulation info.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtNumSteps.Text = _numSteps.ToString();
      txtStepDelay.Text = _stepDelayMs.ToString();
      txtRefreshEvery.Text = _refreshEvery.ToString();
      txtContainerWidth.Text = _containerWidth.ToString("F0");
      txtContainerHeight.Text = _containerHeight.ToString("F0");
      txtContainerDepth.Text = _containerDepth.ToString("F0");
      txtNumParticles.Text = _balls.Count.ToString();
      progressBar.Maximum = _numSteps;
      progressBar.Value = 0;
    }

    /// <summary>
    /// Initializes the 3D scene with camera, lights, particles, and display elements.
    /// </summary>
    private void InitializeScene()
    {
      _myModel3DGroup.Children.Clear();
      myViewport3D.Children.Clear();

      // Only initialize camera on first call
      if (!_sceneInitialized)
      {
        _helper.InitCamera(new Point3D(1350, 1100, 1350));
        _sceneInitialized = true;
      }

      myViewport3D.Camera = _helper._myCamera;

      // Setup lighting
      DirectionalLight light1 = new DirectionalLight(Colors.White, new Vector3D(-0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(light1);

      DirectionalLight light2 = new DirectionalLight(Colors.White, new Vector3D(0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(light2);

      // Add particle spheres
      for (int i = 0; i < _balls.Count; i++)
      {
        _myModel3DGroup.Children.Add(_balls[i]._geomModel);
      }

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };
      myViewport3D.Children.Add(myModelVisual3D);

      // Draw based on display mode
      DrawDisplayElements();

      // Set initial positions
      SetBallsPositionToStep(0);
    }

    /// <summary>
    /// Redraws the scene without resetting camera position.
    /// </summary>
    private void RedrawScene()
    {
      _myModel3DGroup.Children.Clear();
      myViewport3D.Children.Clear();

      myViewport3D.Camera = _helper._myCamera;

      // Setup lighting
      DirectionalLight light1 = new DirectionalLight(Colors.White, new Vector3D(-0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(light1);

      DirectionalLight light2 = new DirectionalLight(Colors.White, new Vector3D(0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(light2);

      // Add particle spheres
      for (int i = 0; i < _balls.Count; i++)
      {
        _myModel3DGroup.Children.Add(_balls[i]._geomModel);
      }

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };
      myViewport3D.Children.Add(myModelVisual3D);

      // Draw based on display mode
      DrawDisplayElements();

      // Restore current position
      SetBallsPositionToStep(_currStep);
    }

    /// <summary>
    /// Draws display elements based on current display mode.
    /// </summary>
    private void DrawDisplayElements()
    {
      switch (_displayMode)
      {
        case DisplayMode.None:
          Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 3, _axisLen);
          break;
        case DisplayMode.BoundingBox:
          DrawBoundingBox();
          break;
        case DisplayMode.CoordinatePlanes:
          DrawCoordinatePlanes();
          break;
      }
    }

    /// <summary>
    /// Draws semi-transparent bounding box planes in the positive quadrant only.
    /// </summary>
    private void DrawBoundingBox()
    {
      double planeThickness = 0.5;
      double opacity = 0.2;

      var planeMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightSkyBlue) { Opacity = opacity });

      // XY plane at Z=0 (floor)
      MeshGeometry3D xyPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(_boxLen / 2, _boxLen / 2, 0), _boxLen, _boxLen, planeThickness);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xyPlaneMesh, planeMaterial));

      // XZ plane at Y=0 (back wall)
      MeshGeometry3D xzPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(_boxLen / 2, 0, _boxLen / 2), _boxLen, planeThickness, _boxLen);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xzPlaneMesh, planeMaterial));

      // YZ plane at X=0 (left wall)
      MeshGeometry3D yzPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(0, _boxLen / 2, _boxLen / 2), planeThickness, _boxLen, _boxLen);
      _myModel3DGroup.Children.Add(new GeometryModel3D(yzPlaneMesh, planeMaterial));

      // Draw coordinate axes
      DrawCoordinateAxes(false);
    }

    /// <summary>
    /// Draws semi-transparent coordinate planes (XY, XZ, YZ) extending in both directions from origin.
    /// </summary>
    private void DrawCoordinatePlanes()
    {
      double planeSize = _boxLen * 2;  // Extend in both directions
      double planeThickness = 0.5;
      double opacity = 0.12;

      // XY plane (Z=0) - Blue tint - centered at origin
      var xyMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightBlue) { Opacity = opacity });
      MeshGeometry3D xyPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(0, 0, 0), planeSize, planeSize, planeThickness);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xyPlaneMesh, xyMaterial));

      // XZ plane (Y=0) - Green tint - centered at origin
      var xzMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightGreen) { Opacity = opacity });
      MeshGeometry3D xzPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(0, 0, 0), planeSize, planeThickness, planeSize);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xzPlaneMesh, xzMaterial));

      // YZ plane (X=0) - Red/Coral tint - centered at origin
      var yzMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightCoral) { Opacity = opacity });
      MeshGeometry3D yzPlaneMesh = Geometries.CreateParallelepiped(
        new Point3D(0, 0, 0), planeThickness, planeSize, planeSize);
      _myModel3DGroup.Children.Add(new GeometryModel3D(yzPlaneMesh, yzMaterial));

      // Draw coordinate axes extending in both directions
      DrawCoordinateAxes(true);
    }

    /// <summary>
    /// Draws the X, Y, Z coordinate axes with distinct colors.
    /// </summary>
    /// <param name="extendBothDirections">If true, axes extend in both positive and negative directions.</param>
    private void DrawCoordinateAxes(bool extendBothDirections)
    {
      double axisLength = _boxLen * 1.1;
      double axisThickness = 2.0;
      double tipRadius = 8.0;

      double startOffset = extendBothDirections ? -axisLength : 0;
      double totalLength = extendBothDirections ? axisLength * 2 : axisLength;
      double centerOffset = extendBothDirections ? 0 : axisLength / 2;

      // X axis - Red
      var xMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Red));
      MeshGeometry3D xAxis = Geometries.CreateParallelepiped(
        new Point3D(centerOffset, 0, 0), totalLength, axisThickness, axisThickness);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xAxis, xMaterial));

      // Y axis - Green
      var yMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Green));
      MeshGeometry3D yAxis = Geometries.CreateParallelepiped(
        new Point3D(0, centerOffset, 0), axisThickness, totalLength, axisThickness);
      _myModel3DGroup.Children.Add(new GeometryModel3D(yAxis, yMaterial));

      // Z axis - Blue
      var zMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
      MeshGeometry3D zAxis = Geometries.CreateParallelepiped(
        new Point3D(0, 0, centerOffset), axisThickness, axisThickness, totalLength);
      _myModel3DGroup.Children.Add(new GeometryModel3D(zAxis, zMaterial));

      // Add spheres at positive axis endpoints as arrow tips
      MeshGeometry3D xTip = Geometries.CreateSphere(new Point3D(axisLength, 0, 0), tipRadius);
      _myModel3DGroup.Children.Add(new GeometryModel3D(xTip, xMaterial));

      MeshGeometry3D yTip = Geometries.CreateSphere(new Point3D(0, axisLength, 0), tipRadius);
      _myModel3DGroup.Children.Add(new GeometryModel3D(yTip, yMaterial));

      MeshGeometry3D zTip = Geometries.CreateSphere(new Point3D(0, 0, axisLength), tipRadius);
      _myModel3DGroup.Children.Add(new GeometryModel3D(zTip, zMaterial));

      // Add smaller spheres at negative endpoints if extending both directions
      if (extendBothDirections)
      {
        double negativeTipRadius = tipRadius * 0.6;

        MeshGeometry3D xNegTip = Geometries.CreateSphere(new Point3D(-axisLength, 0, 0), negativeTipRadius);
        _myModel3DGroup.Children.Add(new GeometryModel3D(xNegTip, xMaterial));

        MeshGeometry3D yNegTip = Geometries.CreateSphere(new Point3D(0, -axisLength, 0), negativeTipRadius);
        _myModel3DGroup.Children.Add(new GeometryModel3D(yNegTip, yMaterial));

        MeshGeometry3D zNegTip = Geometries.CreateSphere(new Point3D(0, 0, -axisLength), negativeTipRadius);
        _myModel3DGroup.Children.Add(new GeometryModel3D(zNegTip, zMaterial));
      }
    }

    /// <summary>
    /// Sets all particle positions to the specified simulation step.
    /// </summary>
    /// <param name="step">The simulation step index.</param>
    private void SetBallsPositionToStep(int step)
    {
      if (step < 0 || step >= _numSteps) return;

      for (int i = 0; i < _balls.Count; i++)
      {
        TranslateTransform3D transform = new TranslateTransform3D
        {
          OffsetX = _balls[i].Pos(step).X,
          OffsetY = _balls[i].Pos(step).Y,
          OffsetZ = _balls[i].Pos(step).Z
        };
        _balls[i]._geomModel.Transform = transform;
      }
    }

    /// <summary>
    /// Helper to parse an integer parameter from a line.
    /// </summary>
    private int LoadIntParamFromLine(string line, string name)
    {
      string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
      if (parts.Length < 2)
        throw new Exception($"Invalid {name} in line: {line}");
      return int.Parse(parts[1]);
    }

    /// <summary>
    /// Helper to parse a double parameter from a line.
    /// </summary>
    private double LoadRealParamFromLine(string line, string name)
    {
      string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
      if (parts.Length < 2)
        throw new Exception($"Invalid {name} in line: {line}");
      return double.Parse(parts[1], CultureInfo.InvariantCulture);
    }

    /// <summary>
    /// Loads particle simulation data from a file.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <returns>True if loading was successful, false otherwise.</returns>
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

        if (lines[0] != "PARTICLE_SIMULATION_DATA_3D")
        {
          MessageBox.Show("Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_3D'.");
          return false;
        }

        _containerWidth = LoadRealParamFromLine(lines[1], "width");
        _containerHeight = LoadRealParamFromLine(lines[2], "height");
        _containerDepth = LoadRealParamFromLine(lines[3], "depth");

        int numBalls = LoadIntParamFromLine(lines[4], "number of balls");
        int lineNumber = 5;

        // Read particle attributes
        for (int i = 0; i < numBalls; i++)
        {
          string[] parts = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 3)
            throw new Exception($"Invalid particle attributes at line {lineNumber}");

          string name = parts[0];
          string color = parts[1];
          double radius = double.Parse(parts[2], CultureInfo.InvariantCulture);

          MeshGeometry3D sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), radius);
          Color ballColor = (Color)ColorConverter.ConvertFromString(color);
          var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(ballColor));
          GeometryModel3D sphereModel = new GeometryModel3D(sphereGeometry, sphereMaterial);

          _balls.Add(new ParticleData3D(name, color, radius, sphereModel));
        }

        // Read number of steps
        _numSteps = LoadIntParamFromLine(lines[lineNumber++], "Number of steps");

        // Read simulation steps
        for (int i = 0; i < _numSteps; i++)
        {
          string[] stepHeader = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (stepHeader[0] != "Step")
            throw new Exception($"Expected 'Step' at line {lineNumber}");

          // Read positions for each particle
          for (int j = 0; j < numBalls; j++)
          {
            string[] posParts = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (posParts.Length < 4)
              throw new Exception($"Invalid position at line {lineNumber}");

            double x = double.Parse(posParts[1], CultureInfo.InvariantCulture);
            double y = double.Parse(posParts[2], CultureInfo.InvariantCulture);
            double z = double.Parse(posParts[3], CultureInfo.InvariantCulture);

            _balls[j].AddPos(new Vector3Cartesian(x, y, z));
          }
        }

        return true;
      }
      catch (Exception ex)
      {
        MessageBox.Show($"Error loading file: {ex.Message}");
        return false;
      }
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
    /// Handles the Start button click.
    /// </summary>
    private void btnStartSim_Click(object sender, RoutedEventArgs e)
    {
      // If paused, resume
      if (_isPaused)
      {
        _isPaused = false;
        btnPauseSim.Content = "Pause";
        return;
      }

      // Parse settings
      if (int.TryParse(txtStepDelay.Text, out int delay))
        _stepDelayMs = Math.Max(1, delay);

      if (int.TryParse(txtRefreshEvery.Text, out int refresh))
        _refreshEvery = Math.Max(1, refresh);

      btnStartSim.IsEnabled = false;
      btnPauseSim.IsEnabled = true;
      btnRestartSim.IsEnabled = false;

      _isRunning = true;

      Task.Run(() => Animate());
    }

    /// <summary>
    /// Handles the Pause button click.
    /// </summary>
    private void btnPauseSim_Click(object sender, RoutedEventArgs e)
    {
      _isPaused = !_isPaused;
      btnPauseSim.Content = _isPaused ? "Resume" : "Pause";

      if (_isPaused)
      {
        btnStartSim.IsEnabled = true;
        btnRestartSim.IsEnabled = true;
      }
    }

    /// <summary>
    /// Handles the Reset button click.
    /// </summary>
    private void btnRestartSim_Click(object sender, RoutedEventArgs e)
    {
      _isRunning = false;
      _isPaused = false;
      _currStep = 0;

      btnStartSim.IsEnabled = true;
      btnPauseSim.IsEnabled = false;
      btnPauseSim.Content = "Pause";
      btnRestartSim.IsEnabled = false;

      txtCurrStep.Text = "0";
      progressBar.Value = 0;

      SetBallsPositionToStep(0);
    }

    /// <summary>
    /// Handles the Look at Center button click.
    /// </summary>
    private void btnLookAtCenter_Click(object sender, RoutedEventArgs e)
    {
      _helper.InitLookAtPoint(new Point3D(_boxLen / 2, _boxLen / 2, _boxLen / 2));
      myViewport3D.InvalidateVisual();
    }

    /// <summary>
    /// Handles the Reset Camera button click.
    /// </summary>
    private void btnResetCamera_Click(object sender, RoutedEventArgs e)
    {
      _helper.ResetCamera();
      myViewport3D.InvalidateVisual();
    }

    /// <summary>
    /// Handles the display mode radio button click.
    /// </summary>
    private void rbDisplayMode_Click(object sender, RoutedEventArgs e)
    {
      if (rbDisplayNone.IsChecked == true)
        _displayMode = DisplayMode.None;
      else if (rbDisplayBoundingBox.IsChecked == true)
        _displayMode = DisplayMode.BoundingBox;
      else if (rbDisplayCoordPlanes.IsChecked == true)
        _displayMode = DisplayMode.CoordinatePlanes;

      if (_sceneInitialized)
        RedrawScene();
    }

    #endregion

    /// <summary>
    /// Animates the particle simulation.
    /// </summary>
    private void Animate()
    {
      while (_currStep < _numSteps && _isRunning)
      {
        // Handle pause
        while (_isPaused && _isRunning)
        {
          Thread.Sleep(50);
        }

        if (!_isRunning) break;

        if (_currStep % _refreshEvery == 0)
        {
          this.Dispatcher.Invoke(() =>
          {
            txtCurrStep.Text = _currStep.ToString();
            progressBar.Value = _currStep;
            SetBallsPositionToStep(_currStep);
          });
        }

        _currStep++;
        Thread.Sleep(_stepDelayMs);
      }

      // Simulation completed
      this.Dispatcher.Invoke(() =>
      {
        if (_currStep >= _numSteps)
        {
          btnStartSim.IsEnabled = false;
          btnPauseSim.IsEnabled = false;
          btnRestartSim.IsEnabled = true;
          progressBar.Value = _numSteps;
        }
        _isRunning = false;
      });
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
  }
}