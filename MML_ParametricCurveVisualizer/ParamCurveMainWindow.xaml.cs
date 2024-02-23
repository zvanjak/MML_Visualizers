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
using MML_VectorFieldVisualizer;
using WPF3DHelperLib;

namespace MML_ParametricCurveVisualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    readonly List<Vector3Cartesian> _curveTrace = new List<Vector3Cartesian>();

    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    Model3DGroup _myModel3DGroup = new Model3DGroup();

    Sphere _sphere = new Sphere();

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

      if (LoadData(fileName))
      {
        // Declare scene objects.

        _helper.InitCamera(new Point3D(180, 80, 150));
        //_helper.InitLights(myModel3DGroup);

        AmbientLight ambLight = new AmbientLight();
        ambLight.Color = Colors.White;
        _myModel3DGroup.Children.Add(ambLight);

        DirectionalLight myDirectionalLight1 = new DirectionalLight(diffuseColor: Colors.White, direction: new Vector3D(-0.31, 0.2, -0.61));
        //myDirectionalLight1.Color = Colors.White;
        //myDirectionalLight1.Direction = new Vector3D(-0.31, 0.2, -0.61);
        _myModel3DGroup.Children.Add(myDirectionalLight1);

        DirectionalLight myDirectionalLight2 = new DirectionalLight();
        myDirectionalLight2.Color = Colors.White;
        myDirectionalLight2.Direction = new Vector3D(0.31, 0.2, -0.61);
        _myModel3DGroup.Children.Add(myDirectionalLight2);

        myViewport3D.Camera = _helper._myCamera;

        ModelVisual3D myModelVisual3D = new ModelVisual3D();
        myModelVisual3D.Content = _myModel3DGroup;

        myViewport3D.Children.Add(myModelVisual3D);

        Utils.DrawCoordSystem(_myModel3DGroup);
        //foreach (var vec in _curveTrace)
        //{
        //    MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(vec.X, vec.Y, vec.Z), 0.2);
        //    var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LimeGreen));
        //    GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);

        //    _myModel3DGroup.Children.Add(sphereModel);
        //}

        MeshGeometry3D line = Geometries.CreateLine2(_curveTrace, 0.12, 10);
        var lineMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.OrangeRed));
        GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        _myModel3DGroup.Children.Add(lineModel);


        MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(0, 0, 0), 2);

        TranslateTransform3D Off = new TranslateTransform3D();
        Off.OffsetX = _sphere.X;
        Off.OffsetY = _sphere.Y;
        Off.OffsetZ = _sphere.Z;

        var sphereMaterial = new DiffuseMaterial(new SolidColorBrush { Color = Color.FromArgb(255, 0, 0, 255) });
        GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);

        sphereModel.Transform = Off;

        _sphere.RefGeomModel = sphereModel;

        _myModel3DGroup.Children.Add(sphereModel);

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

      if (type == "PARAMETRIC_CURVE_CARTESIAN_3D")
      {
        string[] partsT1 = lines[1].Split(' ');
        double t1 = double.Parse(partsT1[1], CultureInfo.InvariantCulture);

        string[] partsT2 = lines[2].Split(' ');
        double t2 = double.Parse(partsT2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[3].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 4; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double t = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[2], CultureInfo.InvariantCulture);
          double z = double.Parse(parts[3], CultureInfo.InvariantCulture);

          Vector3Cartesian pos = new Vector3Cartesian(x, y, z);

          _curveTrace.Add(pos);
        }
      }
      else if (type == "PARAMETRIC_CURVE_CARTESIAN_3D_MULTI")
      {
        MessageBox.Show("PARAMETRIC_CURVE_CARTESIAN_3D_MULTI not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_CARTESIAN_3D_AT_POINTS")
      {
        MessageBox.Show("PARAMETRIC_CURVE_CARTESIAN_3D_AT_POINTS not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_SPHERICAL")
      {
        MessageBox.Show("PARAMETRIC_CURVE_SPHERICAL not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_SPHERICAL_AT_POINTS")
      {
        MessageBox.Show("PARAMETRIC_CURVE_SPHERICAL_AT_POINTS not yet supported");
        return false;
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
    public void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);
    }

    private void cmdAnimate_Click(object sender, RoutedEventArgs e)
    {
      int numSteps = 10000; // Convert.ToInt16(txtNumSteps.Text);
      int refreshEvery = 1; //  Convert.ToInt16(txtRefreshEvery.Text);
      double dt = 1; // Convert.ToDouble(txtDT.Text);

      // setup the scene
      Task.Run(() =>
      {
        Animate(dt, numSteps, refreshEvery);
      });
    }

    private void Animate(double dt, int numSteps, int refreshEvery)
    {

    }
  }
}
