using MML;
using MML_VectorField3D_Visualizer;
using System.Globalization;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using WPF3DHelperLib;

namespace MML_VectorField2D_Visualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class VectorField2D_MainWindow : Window
  {
    string _title = "";

    public VectorField2D_MainWindow()
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
    }

    public List<Vector2Repr> LoadData(string inFileName)
    {
      List<Vector2Repr> ret = new List<Vector2Repr>();

      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return ret;
      }

      string[] lines = File.ReadAllLines(inFileName);

      string type = lines[0];

      _title = lines[1];

      if (type == "VECTOR_FIELD_2D_CARTESIAN")
      {
        for (int i = 2; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);

          double vx = double.Parse(parts[2], CultureInfo.InvariantCulture);
          double vy = double.Parse(parts[3], CultureInfo.InvariantCulture);

          Vector2Cartesian pos = new Vector2Cartesian(x, y);
          Vector2Cartesian vel = new Vector2Cartesian(vx, vy);

          ret.Add(new Vector2Repr(pos, vel));
        }
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
      }

      return ret;
    }
  }
}