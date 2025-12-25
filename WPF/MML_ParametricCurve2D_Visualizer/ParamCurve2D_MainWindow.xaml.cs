using MML_VisualizersBase;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Main window for the 2D Parametric Curve Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides interactive visualization of 2D parametric curves.
  /// It supports loading curve data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Display of single or multiple parametric curves on the same graph</description></item>
  ///   <item><description>Automatic calculation of nice rounded axis tick values</description></item>
  ///   <item><description>Aspect ratio preservation for accurate geometric representation</description></item>
  ///   <item><description>Interactive legend with visibility toggles for each curve</description></item>
  ///   <item><description>Animation of points along curve trajectories with pause/resume/reset</description></item>
  ///   <item><description>Editable graph title</description></item>
  ///   <item><description>Display of parameter range (T min/max) information</description></item>
  ///   <item><description>Responsive resizing</description></item>
  /// </list>
  /// <para>
  /// The application expects data files to be passed as command-line arguments.
  /// Each file should contain parametric curve data in the PARAMETRIC_CURVE_CARTESIAN_2D format.
  /// </para>
  /// </remarks>
  public partial class MainWindow : Window
  {
    private readonly List<LoadedParamCurve2D> _loadedCurves = new List<LoadedParamCurve2D>();
    private readonly CoordSystemParams _coordSystemParams = new CoordSystemParams();
    private readonly CoordSystemStyle _coordSystemStyle = new CoordSystemStyle();
    private string _title = "";

    // Original data bounds (before rounding to nice values)
    private double _dataXMin, _dataXMax, _dataYMin, _dataYMax;
    private double _dataTMin, _dataTMax;

    private bool _preserveAspectRatio = false;
    private bool _isUpdatingTitle = false;

    // Animation state
    private bool _isAnimating = false;
    private bool _isPaused = false;
    private int _currentAnimationStep = 0;
    private CancellationTokenSource? _animationCancellation;
    private readonly List<Ellipse> _animationMarkers = new List<Ellipse>();
    private const double MarkerRadius = 6.0;

    private readonly List<SolidColorBrush> _brushes = Defaults.GetBrushList();

    /// <summary>
    /// Initializes a new instance of the <see cref="MainWindow"/> class.
    /// </summary>
    /// <remarks>
    /// Loads parametric curve data from files specified in command-line arguments.
    /// Each argument should be a path to a valid curve data file.
    /// </remarks>
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

      int numInputs = args.Length - 1;
      for (int i = 0; i < numInputs; i++)
      {
        var fileName = args[i + 1];
        LoadData(fileName, i);
      }
    }

    /// <summary>
    /// Handles the window Loaded event to initialize the visualization.
    /// </summary>
    private void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
      if (_loadedCurves.Count == 0) return;

      CalculateDataBounds();
      InitializeCoordSysParams();
      UpdateLegend();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      LegendWidgetControl.ItemVisibilityChanged += OnLegendItemVisibilityChanged;

      Redraw();
    }

    /// <summary>
    /// Calculates the overall data bounds from all loaded curves.
    /// </summary>
    /// <remarks>
    /// Determines the minimum and maximum X, Y, and T values across all curves
    /// to establish the viewing range for the coordinate system.
    /// </remarks>
    private void CalculateDataBounds()
    {
      if (_loadedCurves.Count == 0) return;

      _dataXMin = _loadedCurves[0].GetMinX();
      _dataXMax = _loadedCurves[0].GetMaxX();
      _dataYMin = _loadedCurves[0].GetMinY();
      _dataYMax = _loadedCurves[0].GetMaxY();
      _dataTMin = _loadedCurves[0].GetMinT();
      _dataTMax = _loadedCurves[0].GetMaxT();

      for (int i = 1; i < _loadedCurves.Count; i++)
      {
        _dataXMin = Math.Min(_dataXMin, _loadedCurves[i].GetMinX());
        _dataXMax = Math.Max(_dataXMax, _loadedCurves[i].GetMaxX());
        _dataYMin = Math.Min(_dataYMin, _loadedCurves[i].GetMinY());
        _dataYMax = Math.Max(_dataYMax, _loadedCurves[i].GetMaxY());
        _dataTMin = Math.Min(_dataTMin, _loadedCurves[i].GetMinT());
        _dataTMax = Math.Max(_dataTMax, _loadedCurves[i].GetMaxT());
      }

      _coordSystemParams._numPoints = _loadedCurves[0].GetNumPoints();
    }

    /// <summary>
    /// Initializes coordinate system parameters based on canvas size and data bounds.
    /// </summary>
    private void InitializeCoordSysParams()
    {
      if (_loadedCurves.Count == 0) return;

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      CoordSystemRenderer.UpdateParamsWithNiceBounds(
        _coordSystemParams, _dataXMin, _dataXMax, _dataYMin, _dataYMax, _preserveAspectRatio);
    }

    /// <summary>
    /// Updates the UI text boxes with current coordinate system bounds and curve info.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtXMin.Text = FormatBoundValue(_coordSystemParams._xMin);
      txtXMax.Text = FormatBoundValue(_coordSystemParams._xMax);
      txtYMin.Text = FormatBoundValue(_coordSystemParams._yMin);
      txtYMax.Text = FormatBoundValue(_coordSystemParams._yMax);
      txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
      txtTMin.Text = FormatBoundValue(_dataTMin);
      txtTMax.Text = FormatBoundValue(_dataTMax);
    }

    /// <summary>
    /// Formats a numeric value for display, using scientific notation when appropriate.
    /// </summary>
    /// <param name="value">The value to format.</param>
    /// <returns>A formatted string representation of the value.</returns>
    private string FormatBoundValue(double value)
    {
      if (Math.Abs(value) >= 10000 || (Math.Abs(value) > 0 && Math.Abs(value) < 0.01))
        return value.ToString("E2");
      if (Math.Abs(value - Math.Round(value)) < 1e-10)
        return ((long)Math.Round(value)).ToString();
      return value.ToString("G6");
    }

    /// <summary>
    /// Redraws the entire visualization including coordinate system and all curves.
    /// </summary>
    private void Redraw()
    {
      if (_loadedCurves.Count == 0) return;

      InitializeCoordSysParams();

      mainCanvas.Children.Clear();
      _animationMarkers.Clear();

      CoordSystemRenderer.Draw(mainCanvas, _coordSystemParams,
        _dataXMin, _dataXMax, _dataYMin, _dataYMax, _coordSystemStyle);

      foreach (var curve in _loadedCurves)
      {
        curve.Draw(mainCanvas, _coordSystemParams);
      }
    }

    /// <summary>
    /// Loads parametric curve data from a file.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <param name="index">The index for assigning default colors.</param>
    /// <returns>True if loading was successful, false otherwise.</returns>
    public bool LoadData(string fileName, int index)
    {
      try
      {
        var (curve, title) = CurveLoader.Load(fileName, index);

        if (curve != null)
        {
          _loadedCurves.Add(curve);
          _title = title ?? "";
          return true;
        }
        return false;
      }
      catch (Exception e)
      {
        MessageBox.Show($"Error loading file {fileName}\nMessage: {e.Message}");
        return false;
      }
    }

    /// <summary>
    /// Handles window resize events to redraw the visualization.
    /// </summary>
    private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      mainCanvas.Children.Clear();
      _animationMarkers.Clear();
      Redraw();
    }

    /// <summary>
    /// Handles the preserve aspect ratio checkbox state change.
    /// </summary>
    /// <remarks>
    /// Preserving aspect ratio is particularly important for parametric curves
    /// like circles, spirals, and Lissajous figures where the true shape matters.
    /// </remarks>
    private void OnPreserveAspectRatioChanged(object sender, RoutedEventArgs e)
    {
      _preserveAspectRatio = chkPreserveAspectRatio.IsChecked ?? false;
      Redraw();
      UpdateUITextBoxes();
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
    /// Handles legend item visibility changes to show/hide curves.
    /// </summary>
    private void OnLegendItemVisibilityChanged(object? sender, EventArgs e)
    {
      UpdateCurveVisibility();
      Redraw();
    }

    /// <summary>
    /// Synchronizes curve visibility with legend checkbox states.
    /// </summary>
    private void UpdateCurveVisibility()
    {
      for (int i = 0; i < _loadedCurves.Count && i < LegendWidgetControl.LegendItems.Count; i++)
      {
        _loadedCurves[i].IsVisible = LegendWidgetControl.LegendItems[i].IsVisible;
      }
    }

    /// <summary>
    /// Populates the legend widget with entries for all loaded curves.
    /// </summary>
    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();

      for (int i = 0; i < _loadedCurves.Count; i++)
      {
        LegendWidgetControl.LegendItems.Add(new LegendItem
        {
          Title = _loadedCurves[i].Title,
          Color = _brushes[i % _brushes.Count],
          IsVisible = true
        });
      }
    }

    #region Animation Controls

    /// <summary>
    /// Creates animated marker ellipses for each visible curve.
    /// </summary>
    private void CreateAnimationMarkers()
    {
      _animationMarkers.Clear();

      for (int i = 0; i < _loadedCurves.Count; i++)
      {
        if (!_loadedCurves[i].IsVisible) 
        {
          _animationMarkers.Add(null!);
          continue;
        }

        Ellipse marker = new Ellipse
        {
          Width = MarkerRadius * 2,
          Height = MarkerRadius * 2,
          Fill = _brushes[i % _brushes.Count],
          Stroke = Brushes.Black,
          StrokeThickness = 1
        };

        _animationMarkers.Add(marker);
        mainCanvas.Children.Add(marker);
      }
    }

    /// <summary>
    /// Updates marker positions to the specified animation step.
    /// </summary>
    /// <param name="step">The animation step (index into curve points).</param>
    private void UpdateMarkerPositions(int step)
    {
      for (int i = 0; i < _loadedCurves.Count; i++)
      {
        if (!_loadedCurves[i].IsVisible || i >= _animationMarkers.Count || _animationMarkers[i] == null)
          continue;

        if (step >= _loadedCurves[i].XValues.Count) continue;

        Point screenPoint = CoordTransform.WorldToScreen(
          _loadedCurves[i].XValues[step], 
          _loadedCurves[i].YValues[step], 
          _coordSystemParams);

        Canvas.SetLeft(_animationMarkers[i], screenPoint.X - MarkerRadius);
        Canvas.SetTop(_animationMarkers[i], screenPoint.Y - MarkerRadius);
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

      Redraw();
      CreateAnimationMarkers();

      // Position markers at current step
      UpdateMarkerPositions(_currentAnimationStep);

      int numSteps = _loadedCurves.Count > 0 ? _loadedCurves[0].GetNumPoints() : 0;

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

      // Redraw scene and create markers at starting position
      Redraw();
      CreateAnimationMarkers();
      UpdateMarkerPositions(0);
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
    /// Animates markers along their respective curve trajectories.
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

        this.Dispatcher.Invoke(() => UpdateMarkerPositions(_currentAnimationStep));

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
  }
}