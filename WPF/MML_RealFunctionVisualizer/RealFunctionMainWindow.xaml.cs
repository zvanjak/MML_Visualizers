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

using MML;
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

        LoadData(fileName, i);
      }

      InitializeCoordSysParams();

      UpdateLegend();

      txtXMin.Text = _coordSystemParams._xMin.ToString();
      txtXMax.Text = _coordSystemParams._xMax.ToString();
      txtYMin.Text = _coordSystemParams._yMin.ToString();
      txtYMax.Text = _coordSystemParams._yMax.ToString();
      txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
      txtTitle.Content = _title;

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

      _coordSystemParams._windowWidth = mainCanvas.Width;
      _coordSystemParams._windowHeight = mainCanvas.Height;

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

    private readonly Brush[] LegendColors = new Brush[]
    {
    Brushes.Black,
    Brushes.Blue,
    Brushes.Red,
    Brushes.Green,
    Brushes.Orange,
    Brushes.Purple,
    Brushes.Brown,
    Brushes.Cyan,
    Brushes.Magenta,
    Brushes.Gray
    };

    private void UpdateLegend()
    {
      // Hide all legend entries by default
      LegendTitle1.Visibility = Visibility.Collapsed;
      LegendTitle2.Visibility = Visibility.Collapsed;
      LegendTitle3.Visibility = Visibility.Collapsed;
      LegendTitle4.Visibility = Visibility.Collapsed;
      LegendTitle5.Visibility = Visibility.Collapsed;
      LegendTitle6.Visibility = Visibility.Collapsed;
      LegendTitle7.Visibility = Visibility.Collapsed;
      LegendTitle8.Visibility = Visibility.Collapsed;
      LegendTitle9.Visibility = Visibility.Collapsed;
      LegendTitle10.Visibility = Visibility.Collapsed;


      LegendColor1.Visibility = Visibility.Collapsed;
      LegendColor2.Visibility = Visibility.Collapsed;
      LegendColor3.Visibility = Visibility.Collapsed;
      LegendColor4.Visibility = Visibility.Collapsed;
      LegendColor5.Visibility = Visibility.Collapsed;
      LegendColor6.Visibility = Visibility.Collapsed;
      LegendColor7.Visibility = Visibility.Collapsed;
      LegendColor8.Visibility = Visibility.Collapsed;
      LegendColor9.Visibility = Visibility.Collapsed;
      LegendColor10.Visibility = Visibility.Collapsed;

      int legendIndex = 0;

      foreach (var func in _loadedFunctions)
      {
        if (func is SingleLoadedFunction slf)
        {
          if (legendIndex < 10)
          {
            SetLegendEntry(legendIndex, slf._title, LegendColors[legendIndex]);
            legendIndex++;
          }
        }
        else if (func is MultiLoadedFunction mlf)
        {
          // Assume mlf has a property for number of functions (dimensions)
          int dim = mlf.GetDimension(); // You may need to implement or expose this
          for (int i = 0; i < dim && legendIndex < 10; i++)
          {
            // If you have per-function titles, use them; otherwise, generate
            string title = mlf._legend[i];
            SetLegendEntry(legendIndex, title, LegendColors[legendIndex]);
            legendIndex++;
          }
        }
      }
    }

    private void SetLegendEntry(int index, string title, Brush color)
    {
      switch (index)
      {
        case 0:
          LegendTitle1.Content = title;
          LegendTitle1.Visibility = Visibility.Visible;
          LegendColor1.Fill = color;
          LegendColor1.Visibility = Visibility.Visible;
          break;
        case 1:
          LegendTitle2.Content = title;
          LegendTitle2.Visibility = Visibility.Visible;
          LegendColor2.Fill = color;
          LegendColor2.Visibility = Visibility.Visible;
          break;
        case 2:
          LegendTitle3.Content = title;
          LegendTitle3.Visibility = Visibility.Visible;
          LegendColor3.Fill = color;
          LegendColor3.Visibility = Visibility.Visible;
          break;
        case 3:
          LegendTitle4.Content = title;
          LegendTitle4.Visibility = Visibility.Visible;
          LegendColor4.Fill = color;
          LegendColor4.Visibility = Visibility.Visible;
          break;
        case 4:
          LegendTitle5.Content = title;
          LegendTitle5.Visibility = Visibility.Visible;
          LegendColor5.Fill = color;
          LegendColor5.Visibility = Visibility.Visible;
          break;
        case 5:
          LegendTitle6.Content = title;
          LegendTitle6.Visibility = Visibility.Visible;
          LegendColor6.Fill = color;
          LegendColor6.Visibility = Visibility.Visible;
          break;
        case 6:
          LegendTitle7.Content = title;
          LegendTitle7.Visibility = Visibility.Visible;
          LegendColor7.Fill = color;
          LegendColor7.Visibility = Visibility.Visible;
          break;
        case 7:
          LegendTitle8.Content = title;
          LegendTitle8.Visibility = Visibility.Visible;
          LegendColor8.Fill = color;
          LegendColor8.Visibility = Visibility.Visible;
          break;
        case 8:
          LegendTitle9.Content = title;
          LegendTitle9.Visibility = Visibility.Visible;
          LegendColor9.Fill = color;
          LegendColor9.Visibility = Visibility.Visible;
          break;
        case 9:
          LegendTitle10.Content = title;
          LegendTitle10.Visibility = Visibility.Visible;
          LegendColor10.Fill = color;
          LegendColor10.Visibility = Visibility.Visible;
          break;
      }
    }
  }
}
