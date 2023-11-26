using System;
using System.Collections.Generic;
using System.Globalization;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    private List<double> _xVals = new List<double>();
    private List<double> _yVals = new List<double>();

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

      if (LoadData(fileName))
      {
        for (int i = 0; i < _xVals.Count; i++)
        {
          DrawPoint(_xVals[i], _yVals[i]);
        }
      }
    }

    void DrawPoint(double x, double y)
    {
      Ellipse circle = new Ellipse();
      circle.Width = 5;
      circle.Height = 5;
      circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, 500 + x * 20 - 2.5);
      Canvas.SetTop(circle, 500 - y * 20 - 2.5);
    }

    public bool LoadData(string inFileName)
    {
      if (File.Exists(inFileName) == false)
      {
        MessageBox.Show("File does not exist: " + inFileName);
        return false;
      } 

      string[] lines = File.ReadAllLines(inFileName);
      string type = lines[0];
      if (type == "REAL_FUNCTION_EQUALLY_SPACED_DETAILED")
      {
        string[] partsX1 = lines[1].Split(' ');
        double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);
        
        string[] partsX2 = lines[2].Split(' ');
        double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

        string[] partsNumPoints = lines[3].Split(' ');
        int numPoints = int.Parse(partsNumPoints[1]);

        for (int i = 4; i < lines.Length; i++)
        {
          string[] parts = lines[i].Split(' ');

          double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
          double y = double.Parse(parts[1], CultureInfo.InvariantCulture);

          _xVals.Add(x);
          _yVals.Add(y);
        }
      }
      else
      {
        MessageBox.Show("Unsupported format: " + type);
        return false;
      }

      return true;
    }
  }
}
