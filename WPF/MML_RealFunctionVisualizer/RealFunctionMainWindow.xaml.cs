using MML;
using MML_VisualizersBase;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Specifies the type of function data loaded from a file.
  /// </summary>
  public enum LoadedType
  {
    /// <summary>A single real function with (x, y) data points.</summary>
    REAL_FUNCTION,
    /// <summary>A function with equally spaced X values.</summary>
    REAL_FUNCTION_EQUALLY_SPACED,
    /// <summary>A function with variable spacing between X values.</summary>
    REAL_FUNCTION_VARIABLE_SPACED,
    /// <summary>Multiple functions sharing the same X values.</summary>
    MULTI_REAL_FUNCTION
  };

  /// <summary>
  /// Main window for the Real Function Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides interactive visualization of real-valued mathematical functions.
  /// It supports loading function data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Display of single or multiple functions on the same graph</description></item>
  ///   <item><description>Automatic calculation of nice rounded axis tick values</description></item>
  ///   <item><description>Optional aspect ratio preservation for accurate geometric representation</description></item>
  ///   <item><description>Interactive legend with visibility toggles for each function</description></item>
  ///   <item><description>Editable graph title</description></item>
  ///   <item><description>Responsive resizing</description></item>
  /// </list>
  /// <para>
  /// The application expects data files to be passed as command-line arguments.
  /// </para>
  /// </remarks>
  public partial class MainWindow : Window
  {
    private readonly List<ILoadedFunction> _loadedFunctions = new List<ILoadedFunction>();
    private readonly CoordSystemParams _coordSystemParams = new CoordSystemParams();
    private readonly CoordSystemStyle _coordSystemStyle = new CoordSystemStyle();
    private string _title = "";

    // Original data bounds (before rounding to nice values)
    private double _dataXMin, _dataXMax, _dataYMin, _dataYMax;

    private bool _preserveAspectRatio = false;
    private bool _isUpdatingTitle = false;

    private readonly List<SolidColorBrush> _brushes = Defaults.GetBrushList();

    /// <summary>
    /// Initializes a new instance of the <see cref="MainWindow"/> class.
    /// </summary>
    /// <remarks>
    /// Loads function data from files specified in command-line arguments.
    /// Each argument should be a path to a valid function data file.
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
      if (_loadedFunctions.Count == 0) return;

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
    /// Calculates the overall data bounds from all loaded functions.
    /// </summary>
    /// <remarks>
    /// Determines the minimum and maximum X and Y values across all functions
    /// to establish the viewing range for the coordinate system.
    /// </remarks>
    private void CalculateDataBounds()
    {
      if (_loadedFunctions.Count == 0) return;

      _dataXMin = _loadedFunctions[0].GetMinX();
      _dataXMax = _loadedFunctions[0].GetMaxX();
      _dataYMin = _loadedFunctions[0].GetMinY();
      _dataYMax = _loadedFunctions[0].GetMaxY();

      for (int i = 1; i < _loadedFunctions.Count; i++)
      {
        _dataXMin = Math.Min(_dataXMin, _loadedFunctions[i].GetMinX());
        _dataXMax = Math.Max(_dataXMax, _loadedFunctions[i].GetMaxX());
        _dataYMin = Math.Min(_dataYMin, _loadedFunctions[i].GetMinY());
        _dataYMax = Math.Max(_dataYMax, _loadedFunctions[i].GetMaxY());
      }

      _coordSystemParams._numPoints = _loadedFunctions[0].GetNumPoints();
    }

    /// <summary>
    /// Initializes coordinate system parameters based on canvas size and data bounds.
    /// </summary>
    private void InitializeCoordSysParams()
    {
      if (_loadedFunctions.Count == 0) return;

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      CoordSystemRenderer.UpdateParamsWithNiceBounds(
        _coordSystemParams, _dataXMin, _dataXMax, _dataYMin, _dataYMax, _preserveAspectRatio);
    }

    /// <summary>
    /// Updates the UI text boxes with current coordinate system bounds.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtXMin.Text = FormatBoundValue(_coordSystemParams._xMin);
      txtXMax.Text = FormatBoundValue(_coordSystemParams._xMax);
      txtYMin.Text = FormatBoundValue(_coordSystemParams._yMin);
      txtYMax.Text = FormatBoundValue(_coordSystemParams._yMax);
      txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
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
    /// Redraws the entire visualization including coordinate system and all functions.
    /// </summary>
    private void Redraw()
    {
      InitializeCoordSysParams();

      mainCanvas.Children.Clear();
      
      CoordSystemRenderer.Draw(mainCanvas, _coordSystemParams, 
        _dataXMin, _dataXMax, _dataYMin, _dataYMax, _coordSystemStyle);

      foreach (var function in _loadedFunctions)
      {
        function.Draw(mainCanvas, _coordSystemParams);
      }
    }

    /// <summary>
    /// Loads function data from a file.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <param name="index">The index for assigning default colors.</param>
    /// <returns>True if loading was successful, false otherwise.</returns>
    public bool LoadData(string fileName, int index)
    {
      try
      {
        var (function, title) = FunctionLoader.Load(fileName, index);
        
        if (function != null)
        {
          _loadedFunctions.Add(function);
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
      Redraw();
    }

    /// <summary>
    /// Handles the preserve aspect ratio checkbox state change.
    /// </summary>
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

      // Update the displayed title when user edits the text
      _title = txtEditableTitle.Text;
      txtTitle.Text = _title;
    }

    /// <summary>
    /// Handles legend item visibility changes to show/hide functions.
    /// </summary>
    private void OnLegendItemVisibilityChanged(object? sender, EventArgs e)
    {
      // Update visibility of functions based on legend item state
      UpdateFunctionVisibility();
      Redraw();
    }

    /// <summary>
    /// Synchronizes function visibility with legend checkbox states.
    /// </summary>
    private void UpdateFunctionVisibility()
    {
      int legendIndex = 0;

      foreach (var func in _loadedFunctions)
      {
        if (func is SingleLoadedFunction slf)
        {
          if (legendIndex < LegendWidgetControl.LegendItems.Count)
          {
            slf.IsVisible = LegendWidgetControl.LegendItems[legendIndex].IsVisible;
          }
          legendIndex++;
        }
        else if (func is MultiLoadedFunction mlf)
        {
          for (int i = 0; i < mlf.Dimension; i++)
          {
            if (legendIndex < LegendWidgetControl.LegendItems.Count)
            {
              mlf.SetFunctionVisibility(i, LegendWidgetControl.LegendItems[legendIndex].IsVisible);
            }
            legendIndex++;
          }
        }
      }
    }

    /// <summary>
    /// Populates the legend widget with entries for all loaded functions.
    /// </summary>
    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();
      int ind = 0;

      foreach (var func in _loadedFunctions)
      {
        for (int i = 0; i < func.Dimension; i++)
        {
          string title = func.GetFunctionTitle(i);
          LegendWidgetControl.LegendItems.Add(new LegendItem
          {
            Title = title,
            Color = _brushes[ind % _brushes.Count],
            IsVisible = true
          });
          ind++;
        }
      }
    }
  }
}
