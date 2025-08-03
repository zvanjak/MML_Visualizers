using MML;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
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
using WPF3DHelperLib;

namespace MML_WorldVisualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    // contains data loaded from JSON files
    private WorldModel _worldModel = new WorldModel();

    // contains data loaded from MML visualizer files
    // - parametric curves in 3D
    // - parametric surfaces in 3D
    // - scalar 2D function ( z = f(x,y) )

    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        //MessageBox.Show("No file name specified.");
        //return;
      }

      // we can have multiple files, so we can load them one by one
      if (args.Length >= 2)
      {
        for (int i = 1; i < args.Length; i++)
        {
          string fileName = args[i];

          // if it is a JSON file, we will load it as WorldModel
          if (fileName.EndsWith(".json", StringComparison.OrdinalIgnoreCase))
          {
            try
            {
              _worldModel = WorldModel.LoadFromJSON(fileName);
              // TODO - handle loaded world model
              // e.g. _worldModel.AddCube(cubeModel);
            }
            catch (Exception ex)
            {
              MessageBox.Show("Error loading JSON file: " + ex.Message);
            }
          }
          if (LoadData(fileName))
          {
            // TODO - handle loaded data
            // e.g. _worldModel.AddCube(cubeModel);
          }
        }
      }

      //var fileName = args[1];
      //if (LoadData(fileName))
      //{
      //}

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();

      _helper.InitCamera(new Point3D(500, 200, 350));
      _helper.InitLights(myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D();
      myModelVisual3D.Content = myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);

      // ajmo dodati sve objekte koje imamo
      Utils.DrawCoordSystem(myModel3DGroup, 0.5, 500);

      CubeModel cubeModel = new CubeModel(new Point3D(0, 0, 0), 10, Colors.Red);

      string json = JsonSerializer.Serialize(cubeModel, new JsonSerializerOptions { WriteIndented = true });
      File.WriteAllText("cube.json", json);

      WorldModel worldModel = new WorldModel();
      worldModel.OriginX = 0;
      worldModel.OriginY = 0;
      worldModel.OriginZ = 0;

      worldModel.AddCube(cubeModel);

      worldModel.SaveToJSON("world.json");

      AddCube(new Point3D(0, 100, 100), 10, Colors.Red);
      AddParalelepiped(new Point3D(50, 50, 50), 20, 30, 40, Colors.Orange);
      AddSphere(new Point3D(100, 100, 100), 10, Colors.Green);
      //AddCylinder(10, 100, Colors.Blue);
      AddVectorArrow(new Point3D(0, 0, 0), new Vector3D(100, 100, 100), 5, 50, Colors.Yellow);
    }

    private bool LoadData(string inFileName)
    {
      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return false;
      }

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];

      // TODO - 3D animacija ... dobije odredjen broj parm krivulja, s istim dT, i onda možeš to gledati
      if (type == "REAL_FUNCTION_EQUALLY_SPACED_DETAILED")
      {
      }
      else if (type == "REAL_FUNCTION_EQUALLY_SPACED")
      {
        MessageBox.Show("REAL_FUNCTION_EQUALLY_SPACED not yet supported");
        return false;
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
        return false;
      }

      return true;
    }

    void AddCube(CubeModel inCube)
    {
      MeshGeometry3D cubeMesh = Geometries.CreateCube(inCube.Center, inCube.SideLength);
      DiffuseMaterial cubeMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.Red));
      //DiffuseMaterial cubeMaterial = new DiffuseMaterial(new SolidColorBrush(inCube.Color));
      GeometryModel3D cubeModel = new GeometryModel3D(cubeMesh, cubeMaterial);
      ModelVisual3D cubeVisual = new ModelVisual3D();
      cubeVisual.Content = cubeModel;
      myViewport3D.Children.Add(cubeVisual);
    }

    void AddCube(Point3D inPnt, double inSize, Color inColor)
    {
      MeshGeometry3D cubeMesh = Geometries.CreateCube(inPnt, inSize);
      DiffuseMaterial cubeMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D cubeModel = new GeometryModel3D(cubeMesh, cubeMaterial);
      ModelVisual3D cubeVisual = new ModelVisual3D();
      cubeVisual.Content = cubeModel;
      myViewport3D.Children.Add(cubeVisual);
    }

    void AddParalelepiped(Point3D inPnt, double inWidth, double inHeight, double inDepth, Color inColor)
    {
      MeshGeometry3D parMesh = Geometries.CreateParallelepiped(inPnt, inWidth, inHeight, inDepth);
      DiffuseMaterial parMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D parModel = new GeometryModel3D(parMesh, parMaterial);
      ModelVisual3D parVisual = new ModelVisual3D();
      parVisual.Content = parModel;
      myViewport3D.Children.Add(parVisual);
    }

    void AddSphere(Point3D inPnt, double inRadius, Color inColor)
    {
      MeshGeometry3D sphereMesh = Geometries.CreateSphere(inPnt, inRadius);
      DiffuseMaterial sphereMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D sphereModel = new GeometryModel3D(sphereMesh, sphereMaterial);
      ModelVisual3D sphereVisual = new ModelVisual3D();
      sphereVisual.Content = sphereModel;
      myViewport3D.Children.Add(sphereVisual);
    }

    // TODO - add tranformation to point with given orientation
    void AddCylinder(double inRadius, double inHeight, Color inColor)
    {
      MeshGeometry3D cylinderMesh = Geometries.CreateCylinder(inRadius, inHeight, 10, 10);
      DiffuseMaterial cylinderMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D cylinderModel = new GeometryModel3D(cylinderMesh, cylinderMaterial);
      ModelVisual3D cylinderVisual = new ModelVisual3D();
      cylinderVisual.Content = cylinderModel;
      myViewport3D.Children.Add(cylinderVisual);
    }

    void AddVectorArrow(Point3D inPnt, Vector3D inVector, double inRadius, double inHeight, Color inColor)
    {
      MeshGeometry3D arrowMesh = Geometries.CreateVectorArrow(inRadius, inHeight, 10, 10, inRadius * 0.8, inHeight * 0.4);
      DiffuseMaterial arrowMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D arrowModel = new GeometryModel3D(arrowMesh, arrowMaterial);
      ModelVisual3D arrowVisual = new ModelVisual3D();
      arrowVisual.Content = arrowModel;
      myViewport3D.Children.Add(arrowVisual);
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

    private void Window_KeyDown(object sender, KeyEventArgs e)
    {
      _helper.Window_KeyDown(myViewport3D, e);
    }
  }
}
