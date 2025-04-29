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

      //Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 3, _axisLen);


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

      if (lines[0] == "PARTICLE_SIMULATION_DATA_2D")
      {
        // read the number of balls
        string[] parts = lines[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length != 2)
          throw new Exception("Invalid number of balls in SimData.txt");

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
          Ball ball = new Ball(name, color, radius);

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
            if (parts.Length != 3)
              throw new Exception("Invalid ball position coords in SimData.txt, line number - " + lineNumber.ToString());

            int index = int.Parse(parts[0]);
            double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
            double y = double.Parse(parts[2], CultureInfo.InvariantCulture);

            Vector2Cartesian pos = new Vector2Cartesian(x, y);

            _balls[j].AddPos(pos);
          }
        }
      }
      else
      {
      }

      return true;
    }
  }
}