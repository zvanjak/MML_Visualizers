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
    List<Vector3Cartesian> _curveTrace = new List<Vector3Cartesian>();

    WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    //PerspectiveCamera _myCamera = new PerspectiveCamera();

    //Point3D _cameraPos = new Point3D(180, 80, 150);
    //Point3D _lookToPos = new Point3D(0, 0, 0);

    //Point3D _startCameraPosRButtonClick;

    //private bool _bLButtonDown = false;
    //private bool _bRightButtonDown = false;

    //private Point _lastMousePos;
    //private Point _startMouseRButtonClick;

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
        Model3DGroup myModel3DGroup = new Model3DGroup();
        ModelVisual3D myModelVisual3D = new ModelVisual3D();

        // Defines the camera used to view the 3D object. In order to view the 3D object,
        // the camera must be positioned and pointed such that the object is within view
        // of the camera.
        _helper._myCamera.Position = _helper._cameraPos;
        _helper._myCamera.LookDirection = Utils.getFrom2Points(_helper._cameraPos, _helper._lookToPos);
        _helper._myCamera.UpDirection = new Vector3D(0, 0, 1);
        _helper._myCamera.FieldOfView = 60;

        myViewport3D.Camera = _helper._myCamera;

        // Define the lights cast in the scene. Without light, the 3D object cannot
        // be seen. Note: to illuminate an object from additional directions, create
        // additional lights.
        AmbientLight ambLight = new AmbientLight();
        ambLight.Color = Colors.Yellow;
        myModel3DGroup.Children.Add(ambLight);

        DirectionalLight myDirectionalLight = new DirectionalLight();
        myDirectionalLight.Color = Colors.White;
        myDirectionalLight.Direction = new Vector3D(-0.61, -0.5, -0.61);
        myModel3DGroup.Children.Add(myDirectionalLight);

        DirectionalLight myDirectionalLight2 = new DirectionalLight();
        myDirectionalLight2.Color = Colors.White;
        myDirectionalLight2.Direction = new Vector3D(0.31, 0.2, -0.61);
        myModel3DGroup.Children.Add(myDirectionalLight2);
        
        Utils.DrawCoordSystem(myModel3DGroup);

        myModelVisual3D.Content = myModel3DGroup;

        myViewport3D.Children.Add(myModelVisual3D);

        foreach (var vec in _curveTrace)
        {
          MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(vec.X, vec.Y, vec.Z), 0.5);
          var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LimeGreen));
          GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);

          myModel3DGroup.Children.Add(sphereModel);
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
      else if (type == "PARAMETRIC_CURVE_CARTESIAN_2D")
      {
        MessageBox.Show("PARAMETRIC_CURVE_CARTESIAN_2D not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_CARTESIAN_2D_AT_POINTS")
      {
        MessageBox.Show("PARAMETRIC_CURVE_CARTESIAN_2D_AT_POINTS not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_CARTESIAN_3D_AT_POINTS")
      {
        MessageBox.Show("PARAMETRIC_CURVE_CARTESIAN_3D_AT_POINTS not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_POLAR")
      {
        MessageBox.Show("PARAMETRIC_CURVE_POLAR not yet supported");
        return false;
      }
      else if (type == "PARAMETRIC_CURVE_POLAR_AT_POINTS")
      {
        MessageBox.Show("PARAMETRIC_CURVE_POLAR_AT_POINTS not yet supported");
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

      //_bLButtonDown = true;
      //_lastMousePos = e.GetPosition(this);
    }

    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonUp();

      // _bLButtonDown = false;
    }

    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonDown(e.GetPosition(this));
      
      //_bRightButtonDown = true;
      //_startMouseRButtonClick = e.GetPosition(this);

      //_startCameraPosRButtonClick = _cameraPos;
    }

    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonUp();
      
      // _bRightButtonDown = false;
    }

    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      _helper.Window_MouseMove(myViewport3D, e.GetPosition(this), sender, e);
      
      //if (_bLButtonDown)
      //{
      //  // morati ćemo uzeti u obzir i smjer gledanja na kraju!
      //  var newPos = e.GetPosition(this);
      //  var diffX = (newPos.X - _lastMousePos.X) / 100.0;
      //  var diffY = (newPos.Y - _lastMousePos.Y) / 100.0;

      //  // uzeti smjer gledanja kao normalu, i kreirati horizontalni x-y koord sustav
      //  // koji određuje koliki su u stvari dx i dy
      //  _cameraPos = new Point3D(_cameraPos.X + diffX, _cameraPos.Y - diffY, _cameraPos.Z);
      //  _lookToPos = new Point3D(_lookToPos.X + diffX, _lookToPos.Y - diffY, _lookToPos.Z);

      //  _myCamera.Position = _cameraPos;

      //  myViewport3D.InvalidateVisual();
      //}

      //if (_bRightButtonDown)
      //{
      //  // za početak, samo ćemo se micati lijevo desno
      //  double diffX = e.GetPosition(this).X - _startMouseRButtonClick.X;
      //  double diffY = e.GetPosition(this).Y - _startMouseRButtonClick.Y;

      //  Debug.WriteLine("Diff {0} - {1}", diffX, diffY);

      //  // znači, moramo zarotirati točku kamere, OKO točke gledanja
      //  double angleX = diffX / 5.0 * Math.PI / 180.0;
      //  double angleY = diffY / 5.0 * Math.PI / 180.0;

      //  Debug.WriteLine("Angle {0} - {1}", angleX, angleY);

      //  // treba oduzeti _lookAtPos, da translatiramo origin
      //  Point3D cam = _startCameraPosRButtonClick;
      //  cam.X -= _lookToPos.X;
      //  cam.Y -= _lookToPos.Y;

      //  // transformiramo u sferne koordinate
      //  Point3Cartesian camPnt = new Point3Cartesian(cam.X, cam.Y, cam.Z);
      //  Point3Spherical outPnt = camPnt.GetSpherical();

      //  //Debug.WriteLine("Polar {0}  Elevation {1}", polar, elevation);

      //  outPnt.Phi += angleX;
      //  outPnt.Theta += angleY;
      //  if (outPnt.Theta < 0.0)
      //    outPnt.Theta = 0.05;

      //  Point3Cartesian newCamPos = outPnt.GetCartesian();

      //  cam.X = newCamPos.X;
      //  cam.Y = newCamPos.Y;
      //  cam.Z = newCamPos.Z;

      //  cam.X += _lookToPos.X;
      //  cam.Y += _lookToPos.Y;

      //  _cameraPos = cam;

      //  _myCamera.Position = _cameraPos;

      //  Debug.WriteLine("{0}  Elevation - {1}", _cameraPos.ToString(), outPnt.Theta);

      //  // treba ažurirati i LookDirection!!!
      //  _myCamera.LookDirection = Utils.getFrom2Points(_cameraPos, _lookToPos);

      //  myViewport3D.InvalidateVisual();
      //}
    }

    public void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);

      //// mijenjamo poziciju kamere da se ili približi ili udalji od točke u koju gledamo
      //Vector3D dir = Utils.getFrom2Points(_cameraPos, _lookToPos);

      //_cameraPos = _cameraPos + (e.Delta / 10.0) * dir;

      //_myCamera.Position = _cameraPos;

      //myViewport3D.InvalidateVisual();
    }
  }
}
