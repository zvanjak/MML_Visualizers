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
    private string _title = "";

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

      InitializeCoordSysParams();
      UpdateLegend();

      txtXMin.Text = _coordSystemParams._xMin.ToString();
      txtXMax.Text = _coordSystemParams._xMax.ToString();
      txtYMin.Text = _coordSystemParams._yMin.ToString();
      txtYMax.Text = _coordSystemParams._yMax.ToString();
      txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
      
      txtTitle.Text = _title;
      
      Redraw();
    }

    private void InitializeCoordSysParams()
    {
      if (_loadedFunctions.Count == 0) return;

      _coordSystemParams._xMin = _loadedFunctions[0].GetMinX();
      _coordSystemParams._xMax = _loadedFunctions[0].GetMaxX();
      _coordSystemParams._yMin = _loadedFunctions[0].GetMinY();
      _coordSystemParams._yMax = _loadedFunctions[0].GetMaxY();
      _coordSystemParams._numPoints = _loadedFunctions[0].GetNumPoints();

      for (int i = 1; i < _loadedFunctions.Count; i++)
      {
        _coordSystemParams._xMin = Math.Min(_coordSystemParams._xMin, _loadedFunctions[i].GetMinX());
        _coordSystemParams._xMax = Math.Max(_coordSystemParams._xMax, _loadedFunctions[i].GetMaxX());
        _coordSystemParams._yMin = Math.Min(_coordSystemParams._yMin, _loadedFunctions[i].GetMinY());
        _coordSystemParams._yMax = Math.Max(_coordSystemParams._yMax, _loadedFunctions[i].GetMaxY());
      }

      _coordSystemParams._windowWidth = mainCanvas.ActualWidth;
      _coordSystemParams._windowHeight = mainCanvas.ActualHeight;

      double midPoint = (_coordSystemParams._xMin + _coordSystemParams._xMax) / 2;
      double midPointY = (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2;

      _coordSystemParams._scaleX = _coordSystemParams._windowWidth / (_coordSystemParams._xMax - _coordSystemParams._xMin) * 0.9;
      _coordSystemParams._scaleY = _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) * 0.9;
      _coordSystemParams._centerX = _coordSystemParams._windowWidth / 2 - midPoint * _coordSystemParams._scaleX;
      _coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + midPointY * _coordSystemParams._scaleY;
    }

    private void Redraw()
    {
      InitializeCoordSysParams();

      mainCanvas.Children.Clear();
      Utils.DrawCoordSystem(mainCanvas, _coordSystemParams, _coordSystemParams._xMin, _coordSystemParams._xMax, _coordSystemParams._yMin, _coordSystemParams._yMax);

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
