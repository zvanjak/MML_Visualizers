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


      for (int i = 0; i < _curves.Count; i++)
      {
        MeshGeometry3D line = Geometries.CreatePolyLine(_curves[i]._curveTrace, _lineWidth, 10);
        DiffuseMaterial lineMaterial = new DiffuseMaterial(_brushes[i]);
        GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        _myModel3DGroup.Children.Add(lineModel);
      }
    }
  }
}