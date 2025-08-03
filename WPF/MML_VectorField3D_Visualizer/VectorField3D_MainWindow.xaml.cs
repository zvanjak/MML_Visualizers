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
using System.Globalization;
using System.IO;

using MML;
using WPF3DHelperLib;

namespace MML_VectorField3D_Visualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    string _title;

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

      var listVecs = LoadData(fileName);

      txtTitle.Text = _title;

      // analizirati podatke
      // naci bounding cube, da se vidi GDJE iscrtavamo polje
      //    podesiti camera pos i direction
      //    podesiti da se i koord osi iscrtaju do max koord
      // naci max i min Norm za sve vektore u listi
      //    u skladu s tim podesiti velicinu vektora

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();

      _helper.InitCamera(new Point3D(180, 80, 150));
      _helper.InitLights(myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D();
      myModelVisual3D.Content = myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(myModel3DGroup, 0.5, 500);

      foreach (var vec in listVecs)
      {
        double baseWidth = vec.Vec.Norm();
        double len = 5 * vec.Vec.Norm();
        MeshGeometry3D vektor = Geometries.CreateVectorArrow(baseWidth, len, 10, 5, baseWidth/2.0, len/3.0);
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
    }

    public List<Vector3Repr> LoadData(string inFileName)
    {
      List<Vector3Repr> ret = new List<Vector3Repr>();

      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return ret;
      }

      string[] lines = File.ReadAllLines(inFileName);
      
      string type = lines[0];
      
      _title = lines[1];
      
      if (type == "VECTOR_FIELD_3D_CARTESIAN")
      {
        for (int i = 2; i < lines.Length; i++)
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

          ret.Add(new Vector3Repr(pos, vel));
        }
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
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
