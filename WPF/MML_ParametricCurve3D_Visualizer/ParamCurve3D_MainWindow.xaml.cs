using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
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
using MML_VectorField3D_Visualizer;
using WPF3DHelperLib;

namespace MML_ParametricCurve3D_Visualizer
{
  class LoadedCurve
  {
    public List<Vector3Cartesian> _curveTrace = new List<Vector3Cartesian>();

  }
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    readonly List<SolidColorBrush> _brushes = new List<SolidColorBrush>() {
        new SolidColorBrush(Colors.Black),
        new SolidColorBrush(Colors.Blue),
        new SolidColorBrush(Colors.Red),
        new SolidColorBrush(Colors.Green),
        new SolidColorBrush(Colors.Orange)
      };

    List<LoadedCurve> _curves = new List<LoadedCurve>();

    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    Model3DGroup _myModel3DGroup = new Model3DGroup();

    List<Sphere> _spheres = new List<Sphere>();

    string _title = "Visualizing param curve";

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
        var fileName2 = args[i + 1];

        var loadedCurve = LoadData(fileName2);
        _curves.Add(loadedCurve);

        Sphere newSphere = new Sphere();
        _spheres.Add(newSphere);
      }

      InitScene();

      //foreach (var vec in _curveTrace)
      //{
      //    MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(vec.X, vec.Y, vec.Z), 0.2);
      //    var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LimeGreen));
      //    GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);

      //    _myModel3DGroup.Children.Add(sphereModel);
      //}

      for (int i = 0; i < _curves.Count; i++)
      {
        MeshGeometry3D line = Geometries.CreatePolyLine(_curves[i]._curveTrace, 0.25, 10);
        DiffuseMaterial lineMaterial = new DiffuseMaterial(_brushes[i]);
        GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        _myModel3DGroup.Children.Add(lineModel);
      }

      txtTitle.Text = _title;
    }

    private void InitScene()
    {
      _helper.InitCamera(new Point3D(350, 100, 350));
      //_helper.InitLights(myModel3DGroup);

      AmbientLight ambLight = new AmbientLight();
      ambLight.Color = Colors.White;
      _myModel3DGroup.Children.Add(ambLight);

      DirectionalLight myDirectionalLight1 = new DirectionalLight(diffuseColor: Colors.White, direction: new Vector3D(-0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight1);

      DirectionalLight myDirectionalLight2 = new DirectionalLight(diffuseColor: Colors.White, direction: new Vector3D(0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight2);

      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D();
      myModelVisual3D.Content = _myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(_myModel3DGroup);
    }

    LoadedCurve LoadData(string inFileName)
    {
      LoadedCurve ret = new LoadedCurve();

      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return ret;
      }

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      if (type == "PARAMETRIC_CURVE_CARTESIAN_3D")
      {
        _title = lines[1];

        string[] partsT1 = lines[2].Split(' ');
        double t1 = double.Parse(partsT1[1], CultureInfo.InvariantCulture);

        string[] partsT2 = lines[3].Split(' ');
        double t2 = double.Parse(partsT2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[4].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 5; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double t = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double x = 5.0 * double.Parse(parts[1], CultureInfo.InvariantCulture);
          double y = 5.0 * double.Parse(parts[2], CultureInfo.InvariantCulture);
          double z = 5.0 * double.Parse(parts[3], CultureInfo.InvariantCulture);

          Vector3Cartesian pos = new Vector3Cartesian(x, y, z);

          ret._curveTrace.Add(pos);
        }
      }

      return ret;
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
      _myModel3DGroup.Children.Clear();

      InitScene();

      for (int i = 0; i < _curves.Count; i++)
      {
        MeshGeometry3D sphereGeometry;
        if ( i==0 )
          sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), 5);
        else
          sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), 2);

        TranslateTransform3D Off = new TranslateTransform3D();
        Off.OffsetX = _spheres[i].X;
        Off.OffsetY = _spheres[i].Y;
        Off.OffsetZ = _spheres[i].Z;

        var sphereMaterial = new DiffuseMaterial(_brushes[i]);
        GeometryModel3D sphereModel = new GeometryModel3D(sphereGeometry, sphereMaterial);

        sphereModel.Transform = Off;

        _spheres[i].RefGeomModel = sphereModel;

        _myModel3DGroup.Children.Add(sphereModel);
      }


      int numSteps = _curves[0]._curveTrace.Count; // Convert.ToInt16(txtNumSteps.Text);
      int refreshEvery = 1; //  Convert.ToInt16(txtRefreshEvery.Text);
      double dt = 1; // Convert.ToDouble(txtDT.Text);

      // run animations in a separate thread
      Task.Run(() =>
      {
        Animate(dt, numSteps, refreshEvery);
      });
    }

    private void Animate(double dt, int numSteps, int refreshEvery)
    {
      for (int t = 0; t < numSteps; t += 1)
      {
        //if (t > 0)
        //{
        //  MeshGeometry3D line = Geometries.CreateSimpleLine(_curveTrace[t - 1], _curveTrace[t], 10, 5);

        //  var lineMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.OrangeRed));
        //  GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        //  _myModel3DGroup.Children.Add(lineModel);
        //}

        for (int i = 0; i < _curves.Count; i++)
        {
          this.Dispatcher.Invoke((Action)(() =>
          {
            TranslateTransform3D Off = new TranslateTransform3D();
            Off.OffsetX = _curves[i]._curveTrace[t].X;
            Off.OffsetY = _curves[i]._curveTrace[t].Y;
            Off.OffsetZ = _curves[i]._curveTrace[t].Z;

            _spheres[i].RefGeomModel.Transform = Off;
          }));
        }

        Thread.Sleep(10);
      }
    }
  }
}
