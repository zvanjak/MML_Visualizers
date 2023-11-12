using MathVisualizer;
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

      LoadData(fileName);

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();
      ModelVisual3D myModelVisual3D = new ModelVisual3D();

      // Defines the camera used to view the 3D object. In order to view the 3D object,
      // the camera must be positioned and pointed such that the object is within view
      // of the camera.
      myPCamera.Position = _cameraPos;
      myPCamera.LookDirection = Calc3D.getFrom2Points(_cameraPos, _lookToPos);
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

      MeshGeometry3D sphere = Geometries.CreateSphere(new Point3D(50, 50, 0), 10);
      var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Red));
      GeometryModel3D sphereModel = new GeometryModel3D(sphere, sphereMaterial);
      myModel3DGroup.Children.Add(sphereModel);

      //MeshGeometry3D stozac = Geometries.CreateDvostrukiStozac(new Point3D(0, 0, 0), 10, 20);
      //var stozacMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
      //GeometryModel3D stozacModel = new GeometryModel3D(stozac, stozacMaterial);
      //myModel3DGroup.Children.Add(stozacModel);

      //MeshGeometry3D valjak = Geometries.CreateValjak(10, 50, 100, 5);
      //var valjakMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
      //GeometryModel3D valjakModel = new GeometryModel3D(valjak, valjakMaterial);
      //myModel3DGroup.Children.Add(valjakModel);

      MeshGeometry3D vektor = Geometries.CreateVectorArrow(10, 50, 10, 5, 7, 25);
      var valjakMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
      GeometryModel3D valjakModel = new GeometryModel3D(vektor, valjakMaterial);

      //valjakModel.Transform = new TranslateTransform3D(50, 50, 0);
      var trans  = new TranslateTransform3D(50, -50, 0);
      var currDir = new Vector3Cartesian(0, 0, 1);
      var newDir = new Vector3Cartesian(1, 1, 3);
      var cross = Vector3Cartesian.VectorProd(currDir, newDir);
      double angle = Vector3Cartesian.VectorAngle(currDir, newDir) / Double.Pi * 180;
      // treba mi vektor na pola put zmeđu z i newDir
      // zbrojiš ih i podijeliš sa 2
      // uzmeš cross produkt (0,0,1) i newDir
      var rot = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(cross.X, cross.Y, cross.Z), angle));
      // valjakModel.Transform = new MatrixTransform3D(trans.Value * rot.Value);
      valjakModel.Transform = rot;
      myModel3DGroup.Children.Add(valjakModel);

      //MeshGeometry3D plane = Geometries.CreatePlane();
      //var planeMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Blue));
      //GeometryModel3D planeModel = new GeometryModel3D(plane, planeMaterial);
      //myModel3DGroup.Children.Add(planeModel);

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


        //Ellipse circle = new Ellipse();
        //circle.Width = 5;
        //circle.Height = 5;
        //circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
        //mainCanvas.Children.Add(circle);
        //Canvas.SetLeft(circle, 500 + x * 20 - 2.5);
        //Canvas.SetTop(circle, 500 - y * 20 - 2.5);
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
        MML.Point3Cartesian camPnt = new Point3Cartesian(cam.X, cam.Y, cam.Z);
        MML.Point3Spherical outPnt = camPnt.GetSpherical();

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
        myPCamera.LookDirection = Calc3D.getFrom2Points(_cameraPos, _lookToPos);

        myViewport3D.InvalidateVisual();
      }
    }

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      // mijenjamo poziciju kamere da se ili približi ili udalji od točke u koju gledamo
      Vector3D dir = Calc3D.getFrom2Points(_cameraPos, _lookToPos);

      _cameraPos = _cameraPos + (e.Delta / 10.0) * dir;

      myPCamera.Position = _cameraPos;

      myViewport3D.InvalidateVisual();
    }

  }
}
