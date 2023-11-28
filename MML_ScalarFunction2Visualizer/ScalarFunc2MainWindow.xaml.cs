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

namespace MML_ScalarFunction2Visualizer
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
        return;
      }

      var fileName = args[1];

      // Declare scene objects.
      Model3DGroup myModel3DGroup = new Model3DGroup();

      _helper.InitCamera(new Point3D(180, 80, 150));
      _helper.InitLights(myModel3DGroup);
      myViewport3D.Camera = _helper._myCamera;

      ModelVisual3D myModelVisual3D = new ModelVisual3D();
      myModelVisual3D.Content = myModel3DGroup;

      myViewport3D.Children.Add(myModelVisual3D);

      Utils.DrawCoordSystem(myModel3DGroup);

      if (LoadData(fileName))
      {
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

      if (type == "SCALAR_FUNCTION_CARTESIAN_2D")
      {
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
  }
}
