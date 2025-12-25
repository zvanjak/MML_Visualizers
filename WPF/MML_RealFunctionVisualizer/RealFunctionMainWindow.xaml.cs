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
  public enum LoadedType
  {
    REAL_FUNCTION,
    REAL_FUNCTION_EQUALLY_SPACED,
    REAL_FUNCTION_VARIABLE_SPACED,
    MULTI_REAL_FUNCTION
  };

  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    private readonly List<ILoadedFunction> _loadedFunctions = new List<ILoadedFunction>();
    private readonly CoordSystemParams _coordSystemParams = new CoordSystemParams();
    private readonly CoordSystemStyle _coordSystemStyle = new CoordSystemStyle();
    private string _title = "";

    // Store original data bounds (before rounding)
    private double _dataXMin, _dataXMax, _dataYMin, _dataYMax;

    // Aspect ratio setting
    private bool _preserveAspectRatio = false;

    // Flag to prevent recursive updates
    private bool _isUpdatingTitle = false;

    private readonly List<SolidColorBrush> _brushes = Defaults.GetBrushList();

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

    private void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
      if (_loadedFunctions.Count == 0) return;

      CalculateDataBounds();
      InitializeCoordSysParams();
      UpdateLegend();
      UpdateUITextBoxes();
      
      // Initialize both title displays
      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;
      
      Redraw();
    }

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

    private void InitializeCoordSysParams()
    {
      if (_loadedFunctions.Count == 0) return;

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      // Use nice rounded bounds with optional aspect ratio preservation
      CoordSystemRenderer.UpdateParamsWithNiceBounds(
        _coordSystemParams, _dataXMin, _dataXMax, _dataYMin, _dataYMax, _preserveAspectRatio);
    }

    private void UpdateUITextBoxes()
    {
      txtXMin.Text = FormatBoundValue(_coordSystemParams._xMin);
      txtXMax.Text = FormatBoundValue(_coordSystemParams._xMax);
      txtYMin.Text = FormatBoundValue(_coordSystemParams._yMin);
      txtYMax.Text = FormatBoundValue(_coordSystemParams._yMax);
      txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
    }

    private string FormatBoundValue(double value)
    {
      if (Math.Abs(value) >= 10000 || (Math.Abs(value) > 0 && Math.Abs(value) < 0.01))
        return value.ToString("E2");
      if (Math.Abs(value - Math.Round(value)) < 1e-10)
        return ((long)Math.Round(value)).ToString();
      return value.ToString("G6");
    }

    private void Redraw()
    {
      InitializeCoordSysParams();

      mainCanvas.Children.Clear();
      
      // Use the new coordinate system renderer with nice ticks
      CoordSystemRenderer.Draw(mainCanvas, _coordSystemParams, 
        _dataXMin, _dataXMax, _dataYMin, _dataYMax, _coordSystemStyle);

      foreach (var function in _loadedFunctions)
      {
        function.Draw(mainCanvas, _coordSystemParams);
      }
    }

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

    private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      mainCanvas.Children.Clear();
      Redraw();
    }

    private void OnPreserveAspectRatioChanged(object sender, RoutedEventArgs e)
    {
      _preserveAspectRatio = chkPreserveAspectRatio.IsChecked ?? false;
      Redraw();
      UpdateUITextBoxes();
    }

    private void OnTitleTextChanged(object sender, TextChangedEventArgs e)
    {
      if (_isUpdatingTitle) return;

      // Update the displayed title when user edits the text
      _title = txtEditableTitle.Text;
      txtTitle.Text = _title;
    }

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
            Color = _brushes[ind % _brushes.Count]
          });
          ind++;
        }
      }
    }
  }
}
