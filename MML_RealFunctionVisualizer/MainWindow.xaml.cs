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
          DrawCoordSystem(-10, 10, -10, 10);
          DrawPoint(_xVals[i], _yVals[i]);
        }
      }
    }

    void DrawCoordSystem(double xMin, double xMax, double yMin, double yMax)
    {
      Line xAxis = new Line();
      xAxis.Stroke = Brushes.Black;
      xAxis.X1 = 0;
      xAxis.Y1 = 400;
      xAxis.X2 = 1000;
      xAxis.Y2 = 400;
      mainCanvas.Children.Add(xAxis);

      Line yAxis = new Line();
      yAxis.Stroke = Brushes.Black;
      yAxis.X1 = 500;
      yAxis.Y1 = 0;
      yAxis.X2 = 500;
      yAxis.Y2 = 1000;
      mainCanvas.Children.Add(yAxis);

      for (int i = 0; i < 1000; i += 20)
      {
        Line xTick = new Line();
        xTick.Stroke = Brushes.Black;
        xTick.X1 = i;
        xTick.Y1 = 400 - 2;
        xTick.X2 = i;
        xTick.Y2 = 400 + 2;
        mainCanvas.Children.Add(xTick);

        Line yTick = new Line();
        yTick.Stroke = Brushes.Black;
        yTick.X1 = 500 - 2;
        yTick.Y1 = i;
        yTick.X2 = 500 + 2;
        yTick.Y2 = i;
        mainCanvas.Children.Add(yTick);
      }

      TextBlock xMinText = new TextBlock();
      xMinText.Text = xMin.ToString();
      mainCanvas.Children.Add(xMinText);
      Canvas.SetLeft(xMinText, 500 + xMin * 20 - 2.5);
      Canvas.SetTop(xMinText, 500 + 5);

      TextBlock xMaxText = new TextBlock();
      xMaxText.Text = xMax.ToString();
      mainCanvas.Children.Add(xMaxText);
      Canvas.SetLeft(xMaxText, 500 + xMax * 20 - 2.5);
      Canvas.SetTop(xMaxText, 500 + 5);

      TextBlock yMinText = new TextBlock();
      yMinText.Text = yMin.ToString();
      mainCanvas.Children.Add(yMinText);
      Canvas.SetLeft(yMinText, 500 - 20);
      Canvas.SetTop(yMinText, 400 - yMin * 20 - 2.5);

      TextBlock yMaxText = new TextBlock();
      yMaxText.Text = yMax.ToString();
      mainCanvas.Children.Add(yMaxText);
      Canvas.SetLeft(yMaxText, 500 - 20);
      Canvas.SetTop(yMaxText, 400 - yMax * 20 - 2.5);
    }
    void DrawPoint(double x, double y)
    {
      Ellipse circle = new Ellipse();
      circle.Width = 5;
      circle.Height = 5;
      circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, 500 + x * 20 - 2.5);
      Canvas.SetTop(circle, 400 - y * 20 - 2.5);
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
      else if (type == "REAL_FUNCTION_EQUALLY_SPACED")
      {
        MessageBox.Show("REAL_FUNCTION_EQUALLY_SPACED not yet supported");
        return false;
      }
      else if (type == "REAL_FUNCTION_VARIABLE_SPACED")
      {
        MessageBox.Show("REAL_FUNCTION_VARIABLE_SPACED not yet supported");
        return false;
      }
      else if (type == "MULTI_REAL_FUNCTION_VARIABLE_SPACED")
      {
        MessageBox.Show("MULTI_REAL_FUNCTION_VARIABLE_SPACED not yet supported");
        return false;
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
