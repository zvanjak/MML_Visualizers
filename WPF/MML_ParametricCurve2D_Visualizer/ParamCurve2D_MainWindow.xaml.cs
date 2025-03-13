﻿using System.Globalization;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MML_ParametricCurve2D_Visualizer
{
  class CoordSystemParams
  {
    public double _xMin;
    public double _xMax;
    public double _yMin;
    public double _yMax;
    public int _numPoints;

    public double _windowWidth = 1000;
    public double _windowHeight = 800;
    public double _centerX = 100;
    public double _centerY = 400;
    public double _scaleX = 40;
    public double _scaleY = 40;
  }

  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    List<LoadedParamCurve2D> _loadedCurves = new List<LoadedParamCurve2D>();
    CoordSystemParams _coordSystemParams = new CoordSystemParams();
    private string _title;

    public MainWindow()
    {
      InitializeComponent();

      // REAL ARGS: func_sin_x_lin_interp.txt sin_func.txt
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

      //txtXMin.Text = _coordSystemParams._xMin.ToString();
      //txtXMax.Text = _coordSystemParams._xMax.ToString();
      //txtYMin.Text = _coordSystemParams._yMin.ToString();
      //txtYMax.Text = _coordSystemParams._yMax.ToString();
      //txtNumPoints.Text = _coordSystemParams._numPoints.ToString();
      txtTitle.Content = _title;

      Redraw();
    }
    private void Redraw()
    {
      //for (int i = 0; i < _loadedFunctions.Count; i++)
      //{
      //  _loadedFunctions[i].Draw(mainCanvas, _coordSystemParams);
      //}
    }

    public bool LoadData(string inFileName, int index)
    {
      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      if (type == "PARAMETRIC_CURVE_CARTESIAN_2D")
      {
        LoadedParamCurve2D newCurve = new LoadedParamCurve2D();

        newCurve._title = lines[1];
        
        string[] partsT1 = lines[2].Split(' ');
        double t1 = double.Parse(partsT1[1], CultureInfo.InvariantCulture);

        string[] partsT2 = lines[3].Split(' ');
        double t2 = double.Parse(partsT2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[4].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 5; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');
          newCurve._tVals.Add(double.Parse(parts[0]));
          newCurve._xVals.Add(double.Parse(parts[1]));
          newCurve._yVals.Add(double.Parse(parts[2]));
        }
        _loadedCurves.Add(newCurve);

        return true;
      }

      return false;
    }

    private void InitializeCoordSysParams()
    {
      //_coordSystemParams._xMin = _loadedFunctions[0].GetMinX();
      //_coordSystemParams._xMax = _loadedFunctions[0].GetMaxX();
      //_coordSystemParams._yMin = _loadedFunctions[0].GetMinY();
      //_coordSystemParams._yMax = _loadedFunctions[0].GetMaxY();
      //_coordSystemParams._numPoints = _loadedFunctions[0].GetNumPoints();

      //for (int i = 1; i < _loadedFunctions.Count; i++)
      //{
      //  _coordSystemParams._xMin = Math.Min(_coordSystemParams._xMin, _loadedFunctions[i].GetMinX());
      //  _coordSystemParams._xMax = Math.Max(_coordSystemParams._xMax, _loadedFunctions[i].GetMaxX());
      //  _coordSystemParams._yMin = Math.Min(_coordSystemParams._yMin, _loadedFunctions[i].GetMinY());
      //  _coordSystemParams._yMax = Math.Max(_coordSystemParams._yMax, _loadedFunctions[i].GetMaxY());
      //}

      //_coordSystemParams._windowWidth = mainCanvas.Width;
      //_coordSystemParams._windowHeight = mainCanvas.Height;

      //// izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
      //// prilagođavanje skaliranja i centra
      //// kod prikazivanja teksta, dok je unutar 0.001, 1000, s decimalama
      //// inače E notacija
      //double midPoint = (_coordSystemParams._xMin + _coordSystemParams._xMax) / 2;
      //double midPointY = (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2;
      //// ako je 0, onda je tocno sredina
      //// ako je manje od 0 , onda je vise sredina prema xMin


      //_coordSystemParams._scaleX = _coordSystemParams._windowWidth / (_coordSystemParams._xMax - _coordSystemParams._xMin) * 0.9;
      //_coordSystemParams._scaleY = _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) * 0.9;
      //_coordSystemParams._centerX = _coordSystemParams._windowWidth / 2 - midPoint * _coordSystemParams._scaleX;
      //_coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + midPointY * _coordSystemParams._scaleY;
      ////_coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2 * _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) + _coordSystemParams._windowHeight / 20;

    }
  }
}