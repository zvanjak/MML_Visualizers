using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using System.Globalization;
using System.IO;

namespace MML_VectorFieldVisualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    PerspectiveCamera myPCamera = new PerspectiveCamera();

    Point3D _cameraPos = new Point3D(180, 80, 150);
    Point3D _lookToPos = new Point3D(0, 0, 0);

    Point3D _startCameraPosRButtonClick;

    private bool _bLButtonDown = false;
    private bool _bRightButtonDown = false;

    private Point _lastMousePos;
    private Point _startMouseRButtonClick;

    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      var fileName = args[1];

      var listVecs = LoadData(fileName);

      // analizirati podatke
      // naci bounding cube, da se vidi GDJE iscrtavamo polje
      //    podesiti camera pos i direction
      //    podesiti da se i koord osi iscrtaju do max koord
      // naci max i min Norm za sve vektore u listi
      //    u skladu s tim podesiti velicinu vektora

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();
      ModelVisual3D myModelVisual3D = new ModelVisual3D();

      // Defines the camera used to view the 3D object. In order to view the 3D object,
      // the camera must be positioned and pointed such that the object is within view
      // of the camera.
      myPCamera.Position = _cameraPos;
      myPCamera.LookDirection = Utils.getFrom2Points(_cameraPos, _lookToPos);
      myPCamera.UpDirection = new Vector3D(0, 0, 1);
      myPCamera.FieldOfView = 60;

      myViewport3D.Camera = myPCamera;

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

      double defAxisWidth = 0.2;
      var axisMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Green));

      MeshGeometry3D axisX = Geometries.CreateParallelepiped(new Point3D(0, 0, 0), 100.0, defAxisWidth, defAxisWidth);
      GeometryModel3D axisXModel = new GeometryModel3D(axisX, axisMaterial);
      myModel3DGroup.Children.Add(axisXModel);

      MeshGeometry3D axisY = Geometries.CreateParallelepiped(new Point3D(0, 0, 0), defAxisWidth, 100, defAxisWidth);
      GeometryModel3D axisYModel = new GeometryModel3D(axisY, axisMaterial);
      myModel3DGroup.Children.Add(axisYModel);

      MeshGeometry3D axisZ = Geometries.CreateParallelepiped(new Point3D(0, 0, 0), defAxisWidth, defAxisWidth, 100);
      GeometryModel3D axisZModel = new GeometryModel3D(axisZ, axisMaterial);
      myModel3DGroup.Children.Add(axisZModel);

      MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(0, 0, 0), 5);
      var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.DarkGreen));
      GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);
      myModel3DGroup.Children.Add(sphereModel);

      foreach (var vec in listVecs)
      {
        double baseWidth = vec.Vec.Norm();
        double len = 5 * vec.Vec.Norm();
        MeshGeometry3D vektor = Geometries.CreateVectorArrow(baseWidth, len, 10, 5, 1, 2);
        var valjakMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
        GeometryModel3D valjakModel = new GeometryModel3D(vektor, valjakMaterial);

        var trans = new TranslateTransform3D(vec.Pos.X, vec.Pos.Y, vec.Pos.Z);
        
        var currDir = new Vector3Cartesian(0, 0, 1);
        var newDir = new Vector3Cartesian(vec.Vec.X, vec.Vec.Y, vec.Vec.Z);
        
        var cross = Vector3Cartesian.VectorProd(currDir, newDir);
        double angle = Vector3Cartesian.VectorAngle(currDir, newDir) / double.Pi * 180;
        var rot = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(cross.X, cross.Y, cross.Z), angle));

        valjakModel.Transform = new MatrixTransform3D(rot.Value * trans.Value);
        //valjakModel.Transform = rot;
        myModel3DGroup.Children.Add(valjakModel);
      }

      // Add the group of models to the ModelVisual3d.
      myModelVisual3D.Content = myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);
    }

    public List<VecRepr> LoadData(string inFileName)
    {
      List<VecRepr> ret = new List<VecRepr>();

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];
      if (type == "VECTOR_FIELD_CARTESIAN")
      { }

      for (int i = 0; i < lines.Length; i++)
      {
        string[] parts = lines[i].Split(' ');

        double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
        double y = double.Parse(parts[1], CultureInfo.InvariantCulture);
        double z = double.Parse(parts[2], CultureInfo.InvariantCulture);

        double vx = double.Parse(parts[3], CultureInfo.InvariantCulture);
        double vy = double.Parse(parts[4], CultureInfo.InvariantCulture);
        double vz = double.Parse(parts[5], CultureInfo.InvariantCulture);

        Vector3Cartesian pos = new Vector3Cartesian(x, y, z);
        Vector3Cartesian vel = new Vector3Cartesian(vx, vy, vz);

        ret.Add(new VecRepr(pos, vel));
      }

      return ret;
    }

    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _bLButtonDown = true;
      _lastMousePos = e.GetPosition(this);
    }

    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _bLButtonDown = false;
    }

    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _bRightButtonDown = true;
      _startMouseRButtonClick = e.GetPosition(this);

      _startCameraPosRButtonClick = _cameraPos;
    }

    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _bRightButtonDown = false;
    }

    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      if (_bLButtonDown)
      {
        // morati ćemo uzeti u obzir i smjer gledanja na kraju!
        var newPos = e.GetPosition(this);
        var diffX = (newPos.X - _lastMousePos.X) / 100.0;
        var diffY = (newPos.Y - _lastMousePos.Y) / 100.0;

        // uzeti smjer gledanja kao normalu, i kreirati horizontalni x-y koord sustav
        // koji određuje koliki su u stvari dx i dy
        _cameraPos = new Point3D(_cameraPos.X + diffX, _cameraPos.Y - diffY, _cameraPos.Z);
        _lookToPos = new Point3D(_lookToPos.X + diffX, _lookToPos.Y - diffY, _lookToPos.Z);

        myPCamera.Position = _cameraPos;

        myViewport3D.InvalidateVisual();
      }

      if (_bRightButtonDown)
      {
        // za početak, samo ćemo se micati lijevo desno
        double diffX = e.GetPosition(this).X - _startMouseRButtonClick.X;
        double diffY = e.GetPosition(this).Y - _startMouseRButtonClick.Y;

        Debug.WriteLine("Diff {0} - {1}", diffX, diffY);

        // znači, moramo zarotirati točku kamere, OKO točke gledanja
        double angleX = diffX / 5.0 * Math.PI / 180.0;
        double angleY = diffY / 5.0 * Math.PI / 180.0;

        Debug.WriteLine("Angle {0} - {1}", angleX, angleY);

        // treba oduzeti _lookAtPos, da translatiramo origin
        Point3D cam = _startCameraPosRButtonClick;
        cam.X -= _lookToPos.X;
        cam.Y -= _lookToPos.Y;

        // transformiramo u sferne koordinate
        Point3Cartesian camPnt = new Point3Cartesian(cam.X, cam.Y, cam.Z);
        Point3Spherical outPnt = camPnt.GetSpherical();

        //Debug.WriteLine("Polar {0}  Elevation {1}", polar, elevation);

        outPnt.Phi += angleX;
        outPnt.Theta += angleY;
        if (outPnt.Theta < 0.0)
          outPnt.Theta = 0.05;

        Point3Cartesian newCamPos = outPnt.GetCartesian();

        cam.X = newCamPos.X;
        cam.Y = newCamPos.Y;
        cam.Z = newCamPos.Z;

        cam.X += _lookToPos.X;
        cam.Y += _lookToPos.Y;

        _cameraPos = cam;

        myPCamera.Position = _cameraPos;

        Debug.WriteLine("{0}  Elevation - {1}", _cameraPos.ToString(), outPnt.Theta);

        // treba ažurirati i LookDirection!!!
        myPCamera.LookDirection = Utils.getFrom2Points(_cameraPos, _lookToPos);

        myViewport3D.InvalidateVisual();
      }
    }

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      // mijenjamo poziciju kamere da se ili približi ili udalji od točke u koju gledamo
      Vector3D dir = Utils.getFrom2Points(_cameraPos, _lookToPos);

      _cameraPos = _cameraPos + (e.Delta / 10.0) * dir;

      myPCamera.Position = _cameraPos;

      myViewport3D.InvalidateVisual();
    }
  }
}
