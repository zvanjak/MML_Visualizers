using System;
using System.Collections.Generic;
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

      double radius = 100;
      double x = 200;
      double y = 200;

      Ellipse circle = new Ellipse();
      circle.Width = radius * 2;
      circle.Height = radius * 2;
      circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, x - radius);
      Canvas.SetTop(circle, y - radius);
    }
  }
}
