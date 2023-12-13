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

namespace MML_RealFunctionVisualizer
{
  enum LoadedType
  {
    REAL_FUNCTION_EQUALLY_SPACED_DETAILED,
    REAL_FUNCTION_EQUALLY_SPACED,
    REAL_FUNCTION_VARIABLE_SPACED,
    MULTI_REAL_FUNCTION_VARIABLE_SPACED
  };

  class CoordSystemParams
  {
    public double _windowWidth = 1000;
    public double _windowHeight = 800;
    public double _centerX = 100;
    public double _centerY = 400;
    public double _scaleX = 40;
    public double _scaleY = 40;
  }
  abstract class LoadedFunction
  {
    public abstract void Draw(Canvas mainCanvas, CoordSystemParams inParams);
  }

  class SingleLoadedFunction : LoadedFunction
  {
    public LoadedType _loadedType;
    public List<double> _xVals = new List<double>();
    public List<double> _yVals = new List<double>();

    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      for (int i = 0; i < _xVals.Count; i++)
      {
        double xMin = _xVals.Min();
        double xMax = _xVals.Max();
        double yMin = _yVals.Min();
        double yMax = _yVals.Max();

        // izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
        // prilagođavanje skaliranja i centra
        // kod prikazivanja tksta, dok je unutar 0.001, 1000, s decimalama
        // inače E notacija
        coordSysParams._scaleX = coordSysParams._windowWidth / (xMax - xMin) * 0.9;
        coordSysParams._scaleY = coordSysParams._windowHeight / (yMax - yMin) * 0.9;
        coordSysParams._centerX = xMin * coordSysParams._windowWidth / (xMax - xMin) + coordSysParams._windowWidth / 20;
        coordSysParams._centerY = -yMin * coordSysParams._windowHeight / (yMax - yMin) - coordSysParams._windowHeight / 20;

        Utils.DrawCoordSystem(mainCanvas, xMin, xMax, yMin, yMax);

        Utils.DrawPoint(mainCanvas, _xVals[i], _yVals[i], Colors.Blue);

        //Rectangle rect = new Rectangle();
        //rect.Width = 100;
        //rect.Height = 100;
        //rect.Fill = new SolidColorBrush(Colors.PaleVioletRed);
        //mainCanvas.Children.Add(rect);
        //Canvas.SetLeft(rect, 100);
        //Canvas.SetTop(rect, 100);
      }
    }
  };

  class MultiLoadedFunction : LoadedFunction
  {
    public MML.Vector _multiFuncX;
    public MML.Matrix _multiFuncY;

    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      double xMin = _multiFuncX.Elements.Min();
      double xMax = _multiFuncX.Elements.Max();
      double yMin = -5; // _multiFuncY.Min();
      double yMax = 15; // _multiFuncY.Max();

      // izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
      // prilagođavanje skaliranja i centra
      // kod prikazivanja tksta, dok je unutar 0.001, 1000, s deimalama
      // inače E notacija
      coordSysParams._scaleX = coordSysParams._windowWidth / (xMax - xMin) * 0.9;
      coordSysParams._scaleY = coordSysParams._windowHeight / (yMax - yMin) * 0.9;
      coordSysParams._centerX = xMin * coordSysParams._windowWidth / (xMax - xMin) + coordSysParams._windowWidth / 20;
      coordSysParams._centerY = -yMin * coordSysParams._windowHeight / (yMax - yMin) - coordSysParams._windowHeight / 20;

      Utils.DrawCoordSystem(mainCanvas, xMin, xMax, yMin, yMax);

      List<Color> colors = new List<Color>();
      colors.Add(Colors.Blue);
      colors.Add(Colors.Red);
      colors.Add(Colors.Green);
      colors.Add(Colors.Yellow);

      for (int i = 0; i < _multiFuncX.Elements.Length; i++)
      {
        for (int j = 0; j < _multiFuncY.Rows; j++)
          Utils.DrawPoint(mainCanvas, _multiFuncX.Elements[i], _multiFuncY.ElemAt(j, i), colors[j]);
      }
    }
  };

  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    List<LoadedFunction> _loadedFunctions = new List<LoadedFunction>(); 
    CoordSystemParams _coordSystemParams = new CoordSystemParams();

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

        LoadData(fileName);
      }

      for (int i = 0; i < _loadedFunctions.Count; i++)
      {
        _loadedFunctions[i].Draw(mainCanvas, _coordSystemParams);
      }
    }

    public bool LoadData(string inFileName)
    {
      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return false;
      } 

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      if (type == "REAL_FUNCTION_EQUALLY_SPACED_DETAILED")
      {
        SingleLoadedFunction slf = new SingleLoadedFunction();

        slf._loadedType = LoadedType.REAL_FUNCTION_EQUALLY_SPACED_DETAILED;


        string[] partsX1 = lines[1].Split(' ');
        double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

        string[] partsX2 = lines[2].Split(' ');
        double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[3].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 4; i < lines.Length; i++)
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
      else if (type == "MULTI_REAL_FUNCTION_VARIABLE_SPACED")
      {
        //_loadedType = LoadedType.MULTI_REAL_FUNCTION_VARIABLE_SPACED;
        MultiLoadedFunction mlf = new MultiLoadedFunction();

        int dim = int.Parse(lines[1]);

        int numPoints = int.Parse(lines[2]);

        double xMin = double.Parse(lines[3], CultureInfo.InvariantCulture);

        double xMax = double.Parse(lines[4], CultureInfo.InvariantCulture);

        mlf._multiFuncX = new MML.Vector(numPoints);
        mlf._multiFuncY = new MML.Matrix(dim, numPoints);

        for (int i = 5; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          mlf._multiFuncX.Elements[i-5] = x;
          for (int j = 0; j < dim; j++)
          {
            double y = double.Parse(parts[j + 1], CultureInfo.InvariantCulture);
            mlf._multiFuncY.SetElemAt(j, i - 5, y);
          }
        }

        _loadedFunctions.Add(mlf);  

        return true;
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
        return false;
      }

      return true;
    }

    private void mainCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
    {

    }

    private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      //MessageBox.Show("Size changed");
      InvalidateVisual();
    }
  }
}
