using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using WPF3DHelperLib;

namespace MML_WorldVisualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        //return;
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

      AddCube(new Point3D(0, 100, 100), 10, Colors.Red);
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

    void AddCube(Point3D inPnt, double inSize, Color inColor)
    {
      MeshGeometry3D cubeMesh = Geometries.CreateCube(inPnt, inSize);
      DiffuseMaterial cubeMaterial = new DiffuseMaterial(new SolidColorBrush(inColor));
      GeometryModel3D cubeModel = new GeometryModel3D(cubeMesh, cubeMaterial);
      ModelVisual3D cubeVisual = new ModelVisual3D();
      cubeVisual.Content = cubeModel;
      myViewport3D.Children.Add(cubeVisual);
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
