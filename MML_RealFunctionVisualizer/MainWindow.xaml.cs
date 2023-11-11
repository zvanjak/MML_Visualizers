using System;
using System.Collections.Generic;
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
    public MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      var fileName = args[1];

      LoadData(fileName);

      double radius = 10;
      double x = 100;
      double y = 100;

      Ellipse circle = new Ellipse();
      circle.Width = radius * 2;
      circle.Height = radius * 2;
      circle.Fill = new SolidColorBrush(Colors.Blue);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, x - radius);
      Canvas.SetTop(circle, y - radius);
    }

    public void LoadData(string inFileName)
    {
      string[] lines = File.ReadAllLines(inFileName);

      foreach (var line in lines)
      {
        string[] parts = line.Split(' ');

        double x = double.Parse(parts[0]);
        double y = double.Parse(parts[1]);

        Ellipse circle = new Ellipse();
        circle.Width = 5;
        circle.Height = 5;
        circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
        mainCanvas.Children.Add(circle);
        Canvas.SetLeft(circle, x - 2.5);
        Canvas.SetTop(circle, y - 2.5);
      }
    }
  }
}
