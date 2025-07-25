﻿using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq.Expressions;
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

using MML;
using MML_VisualizersBase;

namespace MML_ParticleVisualizer2D
{
  public partial class ParticleVisualizer2D_MainWindow : Window
  {
    double _width = 1000;
    double _height = 800;

    double _fixedCanvasWidth = 1000;
    double _fixedCanvasHeight = 1000;

    double _scaleX = 1.0;
    double _scaleY = 1.0;

    List<Ball> _balls = new List<Ball>();
    Ellipse[] _shapes;

    int _numSteps = 0;
    int _stepDelayMiliSec = 100;

    int _currStep = 0;
    bool _isPausedClicked = false;

    public ParticleVisualizer2D_MainWindow()
    {
      InitializeComponent();

      var args = Environment.GetCommandLineArgs();

      if (args.Length < 2)
      {
        MessageBox.Show("No file name specified.");
        return;
      }
      var fileName = args[1];

      if (!LoadData(fileName))
      {
        MessageBox.Show("Error loading data from file " + fileName);
        return;
      }

      // set the title of the window
      this.Title = "Visualizing particle simulation 2D - " + fileName;

      // fill the legend widget with balls info
      LegendWidgetControl.LegendItems.Clear();
      for (int i = 0; i < _balls.Count && i < 10; i++)
      {
        LegendWidgetControl.LegendItems.Add(new LegendItem
        {
          Title = _balls[i].Name,
          Color = new SolidColorBrush((Color)ColorConverter.ConvertFromString(_balls[i].Color))
        });
      }

      txtNumSteps.Text = _numSteps.ToString();
      txtDT.Text = _stepDelayMiliSec.ToString();

      // let's visualize those balls

      // First, the border rectangle
      // we have to determine scaling factor for the balls in x and y direction
      // depending on the size of the container
      // we want to preserve aspect ratio of the container
      double drawRectWidth = 0;
      double drawRectHeight = 0;

      if (_width >= _height)
      {
        drawRectWidth = _fixedCanvasWidth;
        _scaleX = _fixedCanvasWidth / _width;
        _scaleY = _fixedCanvasWidth / _width; // preserve aspect ratio
        drawRectHeight = _height * _scaleY;
      }
      else
      {
        drawRectHeight = _fixedCanvasHeight;
        _scaleY = _fixedCanvasHeight / _height;
        _scaleX = _fixedCanvasHeight / _height; // preserve aspect ratio
        drawRectWidth = _width * _scaleX;
      }

      var border = new Rectangle();
      border.Stroke = Brushes.Black;
      border.Fill = Brushes.AntiqueWhite;
      border.HorizontalAlignment = HorizontalAlignment.Left;
      border.VerticalAlignment = VerticalAlignment.Center;
      border.Width = drawRectWidth;
      border.Height = drawRectHeight;

      MyCanvas.Children.Add(border);

      _shapes = new Ellipse[_balls.Count];

      for (int i = 0; i < _balls.Count; i++)
      {
        _shapes[i] = new Ellipse();

        // set Ellipse Fill to ball color
        string colorName = _balls[i].Color;
        Color color = (Color)ColorConverter.ConvertFromString(colorName);
        _shapes[i].Fill = new SolidColorBrush(color);

        _shapes[i].Width = (_balls[i].Radius * 2) * _scaleX;
        _shapes[i].Height = (_balls[i].Radius * 2) * _scaleY;

        MyCanvas.Children.Add(_shapes[i]);
      }

      SetBallsPosToTimestep(0);

      btnPauseSim.IsEnabled = false;
      btnRestartSim.IsEnabled = false;
    }
    private void SetBallsPosToTimestep(int timeStep)
    {
      for (int i = 0; i < _balls.Count; i++)
      {
        Canvas.SetLeft(_shapes[i], (_balls[i].Pos(timeStep).X1 - _balls[i].Radius) * _scaleX);
        Canvas.SetTop(_shapes[i], (_balls[i].Pos(timeStep).X2 - _balls[i].Radius) * _scaleY);
      }
    }

    public bool LoadData(string fileName)
    {
      if (File.Exists(fileName) == false)
      {
        MessageBox.Show("File does not exist: " + fileName);
        return false;
      }

      try
      {
        // get number of balls
        int numBalls = 0;
        string[] lines = System.IO.File.ReadAllLines(fileName);

        if (lines[0] == "PARTICLE_SIMULATION_DATA_2D")
        {
          // read width and height
          string[] parts1 = lines[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts1.Length != 2)
            throw new Exception("Invalid width!");

          _width = int.Parse(parts1[1]);

          string[] parts2 = lines[2].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts2.Length != 2)
            throw new Exception("Invalid height!");

          _height = int.Parse(parts2[1]);

          // read the number of balls
          string[] parts = lines[3].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

          if (parts.Length != 2)
            throw new Exception("Invalid number of balls in SimData.txt");

          numBalls = int.Parse(parts[1]);

          int lineNumber = 4;

          // read the balls attributes - name, radius, type
          for (int i = 0; i < numBalls; i++)
          {
            parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts.Length != 3)
              throw new Exception("Invalid ball attributes in SimData.txt");

            string name = parts[0];
            string color = parts[1];
            double radius = double.Parse(parts[2], CultureInfo.InvariantCulture);
            Ball ball = new Ball(name, color, radius);

            _balls.Add(ball);
          }

          // read number of steps
          parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length != 2)
            throw new Exception("Invalid number of steps in SimData.txt");
          int numSteps = int.Parse(parts[1]);

          _numSteps = numSteps;

          // read the steps
          for (int i = 0; i < numSteps; i++)
          {
            // we expect "Step 0" together with timing (ie, additional double to specify T)
            parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length != 3)
              throw new Exception("Invalid number of steps in SimData.txt");

            if (parts[0] != "Step")
              throw new Exception("Invalid step in SimData.txt");

            //if (parts[1] != i.ToString())
            //  throw new Exception("Invalid step number in SimData.txt");

            double T = double.Parse(parts[2], CultureInfo.InvariantCulture);

            // read the positions of the balls
            for (int j = 0; j < numBalls; j++)
            {
              parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
              if (parts.Length != 3)
                throw new Exception("Invalid ball position coords in SimData.txt, line number - " + lineNumber.ToString());

              int index = int.Parse(parts[0]);
              double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
              double y = double.Parse(parts[2], CultureInfo.InvariantCulture);

              Vector2Cartesian pos = new Vector2Cartesian(x, y);

              _balls[j].AddPos(pos);
            }
          }
        }
        else
        {
          MessageBox.Show("Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_2D' at the beginning.");
          return false;
        }
      }
      catch (Exception ex)
      {
        MessageBox.Show("Error - " + ex.Message);
        return false;
      }

      return true;
    }

    private void btnStartSim_Click(object sender, RoutedEventArgs e)
    {
      btnRestartSim.IsEnabled = false;
      btnStartSim.IsEnabled = false;
      btnPauseSim.IsEnabled = true;

      _stepDelayMiliSec = int.Parse(txtDT.Text);

      Task.Run(() =>
      {
        Animate(_numSteps, 1);
      });
    }

    private void Animate(int numSteps, int refreshEvery)
    {
      for (int t = _currStep; t < numSteps; t++, _currStep++)
      {
        Thread.Sleep(_stepDelayMiliSec); // Simulate time delay

        if (_isPausedClicked)
        {
          _isPausedClicked = false;
          return;
        }

        if (t % refreshEvery == 0)
        {
          this.Dispatcher.Invoke((Action)(() =>
          {
            txtCurrStep.Text = t.ToString();

            SetBallsPosToTimestep(t);
          }));
        }
      }

      // enable buttons at the end of simulation
      this.Dispatcher.Invoke((Action)(() =>
      {
        btnStartSim.IsEnabled = false;
        btnPauseSim.IsEnabled = false;
        btnRestartSim.IsEnabled = true;
      }));
    }

    private void btnPauseSim_Click(object sender, RoutedEventArgs e)
    {
      _isPausedClicked = true;
      btnRestartSim.IsEnabled = true;
      btnPauseSim.IsEnabled = false;
      btnStartSim.IsEnabled = true;
    }

    private void btnRestartSim_Click(object sender, RoutedEventArgs e)
    {
      btnRestartSim.IsEnabled = false;
      btnPauseSim.IsEnabled = false;
      btnStartSim.IsEnabled = true;

      _currStep = 0;
      txtCurrStep.Text = "0";

      // set all shapes to initial positions
      SetBallsPosToTimestep(0);
    }
  }
}