using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;

using MML;
using WPF3DHelperLib;
using Matrix = MML.Matrix;

namespace MML_ScalarFunction2Visualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    private Matrix _vals;
    private double _xMin;
    private double _xMax;
    private int _numPointsX;
    private double _yMin;
    private double _yMax;
    private int _numPointsY;

    double _scaleX = 10;
    double _scaleY = 10;

    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();


    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }

      var fileName = args[1];

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();

      _helper.InitCamera(new Point3D(180, 80, 150));
      _helper.InitLights(myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D();
      myModelVisual3D.Content = myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(myModel3DGroup);

      if (LoadData(fileName))
      {
        for(int i=0; i<_vals.Rows; i++)
        for (int j = 0; j < _vals.Cols; j++)
        {
          double x = _scaleX * (_xMin + i * (_xMax - _xMin) / _numPointsX);
          double y = _scaleY * (_yMin + j * (_yMax - _yMin) / _numPointsY);
          double z = _vals.ElemAt(i, j);

          MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(x, y, z), 0.5);
          var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LimeGreen));
          GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);

          myModel3DGroup.Children.Add(sphereModel);

          MeshGeometry3D surface = Geometries.CreateSurface(_vals, _xMin, _xMax, _yMin, _yMax, _scaleX, _scaleY);
          var surfaceMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.YellowGreen));
          GeometryModel3D surfaceModel = new GeometryModel3D(surface, surfaceMaterial);

          myModel3DGroup.Children.Add(surfaceModel);

          }
      }
    }

    bool LoadData(string inFileName)
    {
      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return false;
      }

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      if (type == "SCALAR_FUNCTION_CARTESIAN_2D")
      {
        string[] partsX1 = lines[1].Split(' ');
        _xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

        string[] partsX2 = lines[2].Split(' ');
        _xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

        string[] partsNumPointsX = lines[3].Split(' ');
        _numPointsX = int.Parse(partsNumPointsX[1]);

        string[] partsY1 = lines[4].Split(' ');
        _yMin = double.Parse(partsY1[1], CultureInfo.InvariantCulture);

        string[] partsY2 = lines[5].Split(' ');
        _yMax = double.Parse(partsY2[1], CultureInfo.InvariantCulture);

        string[] partsNumPointsY = lines[6].Split(' ');
        _numPointsY = int.Parse(partsNumPointsY[1]);

        _vals = new Matrix(_numPointsX, _numPointsY);

        int count = 0;
        for (int i = 7; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);
          double z = double.Parse(parts[2], CultureInfo.InvariantCulture);

          _vals.SetElemAt(count / _numPointsY, count % _numPointsY, z);
          count++;
        }
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
        return false;
      }

      return true;
    }

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

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);
    }
  }
}
