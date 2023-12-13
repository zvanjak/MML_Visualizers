using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace MML_RealFunctionVisualizer
{
  internal class Utils
  {
    public static void DrawPoint(Canvas mainCanvas, double x, double y, Color inColor)
    {
      Ellipse circle = new Ellipse();
      circle.Width = 5;
      circle.Height = 5;
      circle.Fill = new SolidColorBrush(inColor);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, _centerX + x * _scaleX - 2.5);
      Canvas.SetTop(circle, _centerY - y * _scaleY - 2.5);
    }
    public static void DrawCoordSystem(Canvas mainCanvas, double xMin, double xMax, double yMin, double yMax)
    {
      Line xAxis = new Line();
      xAxis.Stroke = Brushes.Black;
      xAxis.X1 = 0;
      xAxis.Y1 = _centerY;
      xAxis.X2 = _windowWidth;
      xAxis.Y2 = _centerY;
      mainCanvas.Children.Add(xAxis);

      Line yAxis = new Line();
      yAxis.Stroke = Brushes.Black;
      yAxis.X1 = _centerX;
      yAxis.Y1 = 0;
      yAxis.X2 = _centerX;
      yAxis.Y2 = _windowHeight;
      mainCanvas.Children.Add(yAxis);

      int numXTicks = (int)(xMax - xMin);
      for (int i = -numXTicks; i <= numXTicks; i++)
      {
        Line xTick = new Line();
        xTick.Stroke = Brushes.Black;
        xTick.X1 = _centerX + i * _scaleX;
        xTick.Y1 = _centerY - 2;
        xTick.X2 = _centerX + i * _scaleX;
        xTick.Y2 = _centerY + 2;
        mainCanvas.Children.Add(xTick);
      }

      for (int i = -8; i <= 8; i++)
      {
        Line xTick = new Line();
        xTick.Stroke = Brushes.Black;
        xTick.X1 = _centerX - 2;
        xTick.Y1 = _centerY - i * _scaleX;
        xTick.X2 = _centerX + 2;
        xTick.Y2 = _centerY - i * _scaleX;
        mainCanvas.Children.Add(xTick);
      }

      TextBlock xMinText = new TextBlock();
      xMinText.Text = xMin.ToString();
      mainCanvas.Children.Add(xMinText);
      Canvas.SetLeft(xMinText, _centerX + xMin * _scaleX - 2.5);
      Canvas.SetTop(xMinText, _centerY + 5);

      TextBlock xMaxText = new TextBlock();
      xMaxText.Text = xMax.ToString();
      mainCanvas.Children.Add(xMaxText);
      Canvas.SetLeft(xMaxText, _centerX + xMax * _scaleX - 20);
      Canvas.SetTop(xMaxText, _centerY + 5);

      TextBlock yMinText = new TextBlock();
      yMinText.Text = yMin.ToString();
      mainCanvas.Children.Add(yMinText);
      Canvas.SetLeft(yMinText, _centerX - 20);
      Canvas.SetTop(yMinText, _centerY - yMin * _scaleY - 2.5);

      TextBlock yMaxText = new TextBlock();
      yMaxText.Text = yMax.ToString();
      mainCanvas.Children.Add(yMaxText);
      Canvas.SetLeft(yMaxText, _centerX - 20);
      Canvas.SetTop(yMaxText, _centerY - yMax * _scaleY - 2.5);
    }
  }
}
