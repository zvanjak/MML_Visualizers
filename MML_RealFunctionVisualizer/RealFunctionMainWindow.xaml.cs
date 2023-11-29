﻿using System;
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

    double _windowWidth = 1000;
    double _windowHeight = 800;
    double _centerX = 500;
    double _centerY = 400;
    double _scaleX = 40;
    double _scaleY = 40;

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
        // analiza podataka
        for (int i = 0; i < _xVals.Count; i++)
        {
          double xMin = _xVals.Min();
          double xMax = _xVals.Max();
          double yMin = _yVals.Min();
          double yMax = _yVals.Max();

          // izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
          // prilagođavanje skaliranja i centra
          // kod prikazivanja tksta, dok je unutar 0.001, 1000, s deimalama
          // inače E notacija
          _scaleX = _windowWidth / (xMax - xMin) * 0.9;
          _scaleY = _windowHeight / (yMax - yMin) * 0.9;

          DrawCoordSystem(xMin, xMax, yMin, yMax);

          DrawPoint(_xVals[i], _yVals[i]);
        }
      }
    }

    void DrawCoordSystem(double xMin, double xMax, double yMin, double yMax)
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
      for (int i = -numXTicks; i<=numXTicks; i++)
      {
        Line xTick = new Line();
        xTick.Stroke = Brushes.Black;
        xTick.X1 = _centerX + i * _scaleX;
        xTick.Y1 = _centerY - 2;
        xTick.X2 = _centerX + i * _scaleX;
        xTick.Y2 = _centerY + 2;
        mainCanvas.Children.Add(xTick);
      }

      for (int i = -8; i<=8; i++)
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
    void DrawPoint(double x, double y)
    {
      Ellipse circle = new Ellipse();
      circle.Width = 5;
      circle.Height = 5;
      circle.Fill = new SolidColorBrush(Colors.PaleVioletRed);
      mainCanvas.Children.Add(circle);
      Canvas.SetLeft(circle, _centerX + x * _scaleX - 2.5);
      Canvas.SetTop(circle, _centerY - y * _scaleY - 2.5);
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