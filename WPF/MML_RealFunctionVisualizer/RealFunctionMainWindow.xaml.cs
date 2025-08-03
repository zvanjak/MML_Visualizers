using MML;
using MML_VisualizersBase;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  enum LoadedType
  {
    REAL_FUNCTION,
    REAL_FUNCTION_EQUALLY_SPACED,
    REAL_FUNCTION_VARIABLE_SPACED,
    MULTI_REAL_FUNCTION
  };

  abstract class LoadedFunction
  {
    public abstract void Draw(Canvas mainCanvas, CoordSystemParams inParams);
    public abstract double GetMinX();
    public abstract double GetMaxX();
    public abstract double GetMinY();
    public abstract double GetMaxY();
    public abstract int GetNumPoints();
  }

  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    List<LoadedFunction> _loadedFunctions = new List<LoadedFunction>();
    CoordSystemParams _coordSystemParams = new CoordSystemParams();
    private string _title = "";

    readonly List<SolidColorBrush> _brushes = Defaults.GetBrushList();

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

      // izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
      // prilagođavanje skaliranja i centra
      // kod prikazivanja teksta, dok je unutar 0.001, 1000, s decimalama
      // inače E notacija
      double midPoint = (_coordSystemParams._xMin + _coordSystemParams._xMax) / 2;
      double midPointY = (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2;
      // ako je 0, onda je tocno sredina
      // ako je manje od 0 , onda je vise sredina prema xMin


      _coordSystemParams._scaleX = _coordSystemParams._windowWidth / (_coordSystemParams._xMax - _coordSystemParams._xMin) * 0.9;
      _coordSystemParams._scaleY = _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) * 0.9;
      _coordSystemParams._centerX = _coordSystemParams._windowWidth / 2 - midPoint * _coordSystemParams._scaleX;
      _coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + midPointY * _coordSystemParams._scaleY;
      //_coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2 * _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) + _coordSystemParams._windowHeight / 20;

    }

    private void Redraw()
    {
      InitializeCoordSysParams();

      // TODO - add DrawCoordinateSystem here!!!!
      mainCanvas.Children.Clear();
      Utils.DrawCoordSystem(mainCanvas, _coordSystemParams, _coordSystemParams._xMin, _coordSystemParams._xMax, _coordSystemParams._yMin, _coordSystemParams._yMax);

      for (int i = 0; i < _loadedFunctions.Count; i++)
      {
        _loadedFunctions[i].Draw(mainCanvas, _coordSystemParams);
      }
    }

    public bool LoadData(string inFileName, int index)
    {
      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return false;
      }

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      if (type == "REAL_FUNCTION")
      {
        SingleLoadedFunction slf = new SingleLoadedFunction(index);

        slf._loadedType = LoadedType.REAL_FUNCTION;

        slf._title = lines[1];

        _title = slf._title;

        string[] partsX1 = lines[2].Split(' ');
        double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

        string[] partsX2 = lines[3].Split(' ');
        double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[4].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 5; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);

          slf._xVals.Add(x);
          slf._yVals.Add(y);
        }

        _loadedFunctions.Add(slf);
      }
      else if (type == "REAL_FUNCTION_EQUALLY_SPACED")
      {
        MessageBox.Show("REAL_FUNCTION_EQUALLY_SPACED not yet supported");
        //_loadedType = LoadedType.REAL_FUNCTION_EQUALLY_SPACED;

        return false;
      }
      else if (type == "REAL_FUNCTION_VARIABLE_SPACED")
      {
        MessageBox.Show("REAL_FUNCTION_VARIABLE_SPACED not yet supported");
        //_loadedType = LoadedType.REAL_FUNCTION_VARIABLE_SPACED;

        return false;
      }
      else if (type == "MULTI_REAL_FUNCTION")
      {
        //_loadedType = LoadedType.MULTI_REAL_FUNCTION_VARIABLE_SPACED;
        MultiLoadedFunction mlf = new MultiLoadedFunction();

        try
        {
          mlf._title = lines[1];
          int dim = int.Parse(lines[2]);

          string[] legend = new string[dim];
          for (int i = 0; i < dim; i++)
          {
            legend[i] = lines[3 + i];
          }
          mlf._legend = legend;

          string[] partsX1 = lines[3 + dim].Split(' ');
          double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

          string[] partsX2 = lines[4 + dim].Split(' ');
          double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

          string[] partsNumPoints = lines[5 + dim].Split(' ');
          int numPoints = int.Parse(partsNumPoints[1], CultureInfo.InvariantCulture);

          _title = mlf._title;

          mlf._multiFuncX = new MML.Vector(numPoints);
          mlf._multiFuncY = new MML.Matrix(dim, numPoints);

          int cnt = 0;
          for (int i = 6 + dim; i < lines.Length; i++, cnt++)
          {
            string[] parts = lines[i].Split(' ');

            double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
            mlf._multiFuncX.Elements[cnt] = x;
            for (int j = 0; j < dim; j++)
            {
              double y = double.Parse(parts[j + 1], CultureInfo.InvariantCulture);
              mlf._multiFuncY.SetElemAt(j, cnt, y);
            }
          }

          _loadedFunctions.Add(mlf);

          return true;
        }
        catch (Exception e)
        {
          MessageBox.Show("Error loading file " + inFileName + "\nMessage: " + e.Message);
          return false;
        }
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
        return false;
      }

      return true;
    }

    private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      // clear canvas
      mainCanvas.Children.Clear();

      // redraw
      Redraw();
    }

    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();
      int ind = 0;

      foreach (var func in _loadedFunctions)
      {
        if (func is SingleLoadedFunction slf)
        {
          LegendWidgetControl.LegendItems.Add(new LegendItem
          {
            Title = slf._title,
            Color = _brushes[ind % _brushes.Count]
          });
          ind++;
        }
        else if (func is MultiLoadedFunction mlf)
        {
          // Assume mlf has a property for number of functions (dimensions)
          int dim = mlf.GetDimension(); // You may need to implement or expose this
          for (int i = 0; i < dim && i < 10; i++)
          {
            // If you have per-function titles, use them; otherwise, generate
            string title = (mlf._legend != null && i < mlf._legend.Length) ? mlf._legend[i] : $"Function {i + 1}";
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
}
