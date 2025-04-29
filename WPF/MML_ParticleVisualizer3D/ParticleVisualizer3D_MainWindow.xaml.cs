using MML;
using System.Globalization;
using System.Text;
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
using WPF3DHelperLib;

namespace MML_ParticleVisualizer3D
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class ParticleVisualizer3D_MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    Point3D _cameraPoint = new Point3D(350, 100, 350);
    double _axisWidth = 0.5;
    double _axisLen = 500;
    double _lineWidth = 0.25;

    int _numSteps = 0;
    int _stepDelayMiliSec = 100;

    List<ParticleData3D> _balls = new List<ParticleData3D>();

    Model3DGroup _myModel3DGroup = new Model3DGroup();

    public ParticleVisualizer3D_MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }
      var fileName = args[1];

      if (!LoadData(fileName))
      {
        MessageBox.Show("Error loading data from file.");
        return;
      }

      InitScene();
    }


    private void InitScene()
    {
      _myModel3DGroup.Children.Clear();

      _helper.InitCamera(_cameraPoint);
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

      Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 3, _axisLen);


      //for (int i = 0; i < _curves.Count; i++)
      //{
      //  MeshGeometry3D line = Geometries.CreatePolyLine(_curves[i]._curveTrace, _lineWidth, 10);
      //  DiffuseMaterial lineMaterial = new DiffuseMaterial(_brushes[i]);
      //  GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

      //  _myModel3DGroup.Children.Add(lineModel);
      //}
    }

    public bool LoadData(string fileName)
    {
      // get number of balls
      int numBalls = 0;
      string[] lines = System.IO.File.ReadAllLines(fileName);

      if (lines[0] == "PARTICLE_SIMULATION_DATA_3D")
      {
        // read the number of balls
        string[] parts = lines[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length != 2)
          throw new Exception("Invalid number of balls in " + fileName);

        numBalls = int.Parse(parts[1]);

        int lineNumber = 2;

        // read the balls attributes - name, radius, type
        for (int i = 0; i < numBalls; i++)
        {
          parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

          if (parts.Length != 3)
            throw new Exception("Invalid ball attributes in SimData.txt");

          string name = parts[0];
          string color = parts[1];
          double radius = double.Parse(parts[2], CultureInfo.InvariantCulture);
          ParticleData3D ball = new ParticleData3D(name, color, radius);

          _balls.Add(ball);
        }

        // read number of steps
        parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length != 2)
          throw new Exception("Invalid number of steps in SimData.txt");
        int numSteps = int.Parse(parts[1]);

        _numSteps = numSteps;

        // read the steps
        for (int i = 0; i < numSteps; i++)
        {
          // we expect "Step 0" together with timing (ie, additional double to specify T)
          parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length != 3)
            throw new Exception("Invalid number of steps in SimData.txt");

          if (parts[0] != "Step")
            throw new Exception("Invalid step in SimData.txt");
          if (parts[1] != i.ToString())
            throw new Exception("Invalid step number in SimData.txt");
          double T = double.Parse(parts[2], CultureInfo.InvariantCulture);

          // read the positions of the balls
          for (int j = 0; j < numBalls; j++)
          {
            parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length != 4)
              throw new Exception("Invalid ball position coords in SimData.txt, line number - " + lineNumber.ToString());

            int index = int.Parse(parts[0]);
            double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
            double y = double.Parse(parts[2], CultureInfo.InvariantCulture);
            double z = double.Parse(parts[3], CultureInfo.InvariantCulture);

            Vector3Cartesian pos = new Vector3Cartesian(x, y, z);

            _balls[j].AddPos(pos);
          }
        }
      }
      else
      {
        MessageBox.Show("Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_3D' at the beginning.");
        return false;
      }

      return true;
    }


    private void cmdAnimate_Click(object sender, RoutedEventArgs e)
    {
      _myModel3DGroup.Children.Clear();

      InitScene();

      //for (int i = 0; i < _curves.Count; i++)
      //{
      //  MeshGeometry3D sphereGeometry;
      //  if (i == 0)
      //    sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), 5);
      //  else
      //    sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), 2);

      //  TranslateTransform3D Off = new TranslateTransform3D();
      //  Off.OffsetX = _spheres[i].X;
      //  Off.OffsetY = _spheres[i].Y;
      //  Off.OffsetZ = _spheres[i].Z;

      //  var sphereMaterial = new DiffuseMaterial(_brushes[i]);
      //  GeometryModel3D sphereModel = new GeometryModel3D(sphereGeometry, sphereMaterial);

      //  sphereModel.Transform = Off;

      //  _spheres[i].RefGeomModel = sphereModel;

      //  _myModel3DGroup.Children.Add(sphereModel);
      //}


      int refreshEvery = 1; //  Convert.ToInt16(txtRefreshEvery.Text);
      double dt = 1; // Convert.ToDouble(txtDT.Text);

      // run animations in a separate thread
      Task.Run(() =>
      {
        Animate(dt, _numSteps, refreshEvery);
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

        //for (int i = 0; i < _curves.Count; i++)
        //{
        //  this.Dispatcher.Invoke((Action)(() =>
        //  {
        //    TranslateTransform3D Off = new TranslateTransform3D();
        //    Off.OffsetX = _curves[i]._curveTrace[t].X;
        //    Off.OffsetY = _curves[i]._curveTrace[t].Y;
        //    Off.OffsetZ = _curves[i]._curveTrace[t].Z;

        //    _spheres[i].RefGeomModel.Transform = Off;
        //  }));
        //}

        Thread.Sleep(10);
      }
    }


    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonDown(e.GetPosition(this));

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonUp();

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonDown(e.GetPosition(this));

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonUp();

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      _helper.Window_MouseMove(myViewport3D, e.GetPosition(this), sender, e);

      _cameraPoint = _helper._cameraPos;
    }
    public void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);

      _cameraPoint = _helper._cameraPos;
    }

    private void btnIncWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 1.1;
      InitScene();
    }

    private void btnDecWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 0.9;
      InitScene();
    }
  }
}