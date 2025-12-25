using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

using MML;
using MML_VisualizersBase;

namespace MML_ParticleVisualizer2D
{
  /// <summary>
  /// Main window for the 2D Particle Visualizer application.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This window provides visualization of 2D particle simulations.
  /// It supports loading simulation data from text files and provides features including:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Playback of particle simulation trajectories</description></item>
  ///   <item><description>Start, Pause, and Reset controls</description></item>
  ///   <item><description>Adjustable animation speed and refresh rate</description></item>
  ///   <item><description>Progress bar showing simulation progress</description></item>
  ///   <item><description>Legend showing particle names and colors</description></item>
  ///   <item><description>Editable simulation title</description></item>
  /// </list>
  /// </remarks>
  public partial class ParticleVisualizer2D_MainWindow : Window
  {
    private double _containerWidth = 1000;
    private double _containerHeight = 800;

    private readonly double _fixedCanvasWidth = 1000;
    private readonly double _fixedCanvasHeight = 1000;

    private double _scaleX = 1.0;
    private double _scaleY = 1.0;

    private readonly List<Ball> _balls = new List<Ball>();
    private Ellipse[] _shapes = Array.Empty<Ellipse>();

    private int _numSteps = 0;
    private int _stepDelayMs = 10;
    private int _refreshEvery = 1;

    private int _currStep = 0;
    private bool _isPaused = false;
    private bool _isRunning = false;

    private string _title = "Particle Simulation 2D";
    private bool _isUpdatingTitle = false;

    /// <summary>
    /// Initializes a new instance of the <see cref="ParticleVisualizer2D_MainWindow"/> class.
    /// </summary>
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
        MessageBox.Show($"Error loading data from file {fileName}");
        return;
      }

      _title = $"Particle Simulation - {System.IO.Path.GetFileNameWithoutExtension(fileName)}";
      
      InitializeVisualization();
      UpdateLegend();
      UpdateUITextBoxes();

      txtTitle.Text = _title;
      _isUpdatingTitle = true;
      txtEditableTitle.Text = _title;
      _isUpdatingTitle = false;

      // Set initial button states
      btnPauseSim.IsEnabled = false;
      btnRestartSim.IsEnabled = false;
    }

    /// <summary>
    /// Updates the legend with particle information.
    /// </summary>
    private void UpdateLegend()
    {
      LegendWidgetControl.LegendItems.Clear();

      for (int i = 0; i < _balls.Count && i < 10; i++)
      {
        LegendWidgetControl.LegendItems.Add(new LegendItem
        {
          Title = _balls[i].Name,
          Color = new SolidColorBrush((Color)ColorConverter.ConvertFromString(_balls[i].Color)),
          IsVisible = true
        });
      }
    }

    /// <summary>
    /// Updates the UI text boxes with current simulation info.
    /// </summary>
    private void UpdateUITextBoxes()
    {
      txtNumSteps.Text = _numSteps.ToString();
      txtStepDelay.Text = _stepDelayMs.ToString();
      txtRefreshEvery.Text = _refreshEvery.ToString();
      txtContainerWidth.Text = _containerWidth.ToString("F0");
      txtContainerHeight.Text = _containerHeight.ToString("F0");
      txtNumParticles.Text = _balls.Count.ToString();
      progressBar.Maximum = _numSteps;
      progressBar.Value = 0;
    }

    /// <summary>
    /// Initializes the visualization canvas with container and particles.
    /// </summary>
    private void InitializeVisualization()
    {
      MyCanvas.Children.Clear();

      // Calculate scaling to preserve aspect ratio
      double drawRectWidth, drawRectHeight;

      if (_containerWidth >= _containerHeight)
      {
        drawRectWidth = _fixedCanvasWidth;
        _scaleX = _fixedCanvasWidth / _containerWidth;
        _scaleY = _scaleX; // preserve aspect ratio
        drawRectHeight = _containerHeight * _scaleY;
      }
      else
      {
        drawRectHeight = _fixedCanvasHeight;
        _scaleY = _fixedCanvasHeight / _containerHeight;
        _scaleX = _scaleY; // preserve aspect ratio
        drawRectWidth = _containerWidth * _scaleX;
      }

      // Draw container border
      var border = new Rectangle
      {
        Stroke = Brushes.Black,
        Fill = Brushes.AntiqueWhite,
        Width = drawRectWidth,
        Height = drawRectHeight
      };
      MyCanvas.Children.Add(border);

      // Create particle shapes
      _shapes = new Ellipse[_balls.Count];

      for (int i = 0; i < _balls.Count; i++)
      {
        Color color = (Color)ColorConverter.ConvertFromString(_balls[i].Color);

        _shapes[i] = new Ellipse
        {
          Fill = new SolidColorBrush(color),
          Width = (_balls[i].Radius * 2) * _scaleX,
          Height = (_balls[i].Radius * 2) * _scaleY
        };

        MyCanvas.Children.Add(_shapes[i]);
      }

      SetBallsPositionToStep(0);
    }

    /// <summary>
    /// Sets all particle positions to the specified simulation step.
    /// </summary>
    /// <param name="step">The simulation step index.</param>
    private void SetBallsPositionToStep(int step)
    {
      for (int i = 0; i < _balls.Count; i++)
      {
        Canvas.SetLeft(_shapes[i], (_balls[i].Pos(step).X1 - _balls[i].Radius) * _scaleX);
        Canvas.SetTop(_shapes[i], (_balls[i].Pos(step).X2 - _balls[i].Radius) * _scaleY);
      }
    }

    /// <summary>
    /// Loads particle simulation data from a file.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <returns>True if loading was successful, false otherwise.</returns>
    private bool LoadData(string fileName)
    {
      if (!File.Exists(fileName))
      {
        MessageBox.Show($"File does not exist: {fileName}");
        return false;
      }

      try
      {
        string[] lines = File.ReadAllLines(fileName);

        if (lines[0] != "PARTICLE_SIMULATION_DATA_2D")
        {
          MessageBox.Show("Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_2D'.");
          return false;
        }

        // Read container dimensions
        string[] widthParts = lines[1].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        _containerWidth = double.Parse(widthParts[1], CultureInfo.InvariantCulture);

        string[] heightParts = lines[2].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        _containerHeight = double.Parse(heightParts[1], CultureInfo.InvariantCulture);

        // Read number of particles
        string[] numBallsParts = lines[3].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        int numBalls = int.Parse(numBallsParts[1]);

        int lineNumber = 4;

        // Read particle attributes
        for (int i = 0; i < numBalls; i++)
        {
          string[] parts = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length < 3)
            throw new Exception($"Invalid particle attributes at line {lineNumber}");

          string name = parts[0];
          string color = parts[1];
          double radius = double.Parse(parts[2], CultureInfo.InvariantCulture);

          _balls.Add(new Ball(name, color, radius));
        }

        // Read number of steps
        string[] stepsParts = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        _numSteps = int.Parse(stepsParts[1]);

        // Read simulation steps
        for (int i = 0; i < _numSteps; i++)
        {
          string[] stepHeader = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
          if (stepHeader[0] != "Step")
            throw new Exception($"Expected 'Step' at line {lineNumber}");

          // Read positions for each particle
          for (int j = 0; j < numBalls; j++)
          {
            string[] posParts = lines[lineNumber++].Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (posParts.Length < 3)
              throw new Exception($"Invalid position at line {lineNumber}");

            double x = double.Parse(posParts[1], CultureInfo.InvariantCulture);
            double y = double.Parse(posParts[2], CultureInfo.InvariantCulture);

            _balls[j].AddPos(new Vector2Cartesian(x, y));
          }
        }

        return true;
      }
      catch (Exception ex)
      {
        MessageBox.Show($"Error loading file: {ex.Message}");
        return false;
      }
    }

    #region Event Handlers

    /// <summary>
    /// Handles changes to the editable title text box.
    /// </summary>
    private void OnTitleTextChanged(object sender, TextChangedEventArgs e)
    {
      if (_isUpdatingTitle) return;

      _title = txtEditableTitle.Text;
      txtTitle.Text = _title;
    }

    /// <summary>
    /// Handles the Start button click.
    /// </summary>
    private void btnStartSim_Click(object sender, RoutedEventArgs e)
    {
      // If paused, resume
      if (_isPaused)
      {
        _isPaused = false;
        btnPauseSim.Content = "Pause";
        return;
      }

      // Parse settings
      if (int.TryParse(txtStepDelay.Text, out int delay))
        _stepDelayMs = Math.Max(1, delay);

      if (int.TryParse(txtRefreshEvery.Text, out int refresh))
        _refreshEvery = Math.Max(1, refresh);

      btnStartSim.IsEnabled = false;
      btnPauseSim.IsEnabled = true;
      btnRestartSim.IsEnabled = false;

      _isRunning = true;

      Task.Run(() => Animate());
    }

    /// <summary>
    /// Handles the Pause button click.
    /// </summary>
    private void btnPauseSim_Click(object sender, RoutedEventArgs e)
    {
      _isPaused = !_isPaused;
      btnPauseSim.Content = _isPaused ? "Resume" : "Pause";
      
      if (_isPaused)
      {
        btnStartSim.IsEnabled = true;
        btnRestartSim.IsEnabled = true;
      }
    }

    /// <summary>
    /// Handles the Reset button click.
    /// </summary>
    private void btnRestartSim_Click(object sender, RoutedEventArgs e)
    {
      _isRunning = false;
      _isPaused = false;
      _currStep = 0;

      btnStartSim.IsEnabled = true;
      btnPauseSim.IsEnabled = false;
      btnPauseSim.Content = "Pause";
      btnRestartSim.IsEnabled = false;

      txtCurrStep.Text = "0";
      progressBar.Value = 0;

      SetBallsPositionToStep(0);
    }

    #endregion

    /// <summary>
    /// Animates the particle simulation.
    /// </summary>
    private void Animate()
    {
      while (_currStep < _numSteps && _isRunning)
      {
        // Handle pause
        while (_isPaused && _isRunning)
        {
          Thread.Sleep(50);
        }

        if (!_isRunning) break;

        if (_currStep % _refreshEvery == 0)
        {
          this.Dispatcher.Invoke(() =>
          {
            txtCurrStep.Text = _currStep.ToString();
            progressBar.Value = _currStep;
            SetBallsPositionToStep(_currStep);
          });
        }

        _currStep++;
        Thread.Sleep(_stepDelayMs);
      }

      // Simulation completed
      this.Dispatcher.Invoke(() =>
      {
        if (_currStep >= _numSteps)
        {
          btnStartSim.IsEnabled = false;
          btnPauseSim.IsEnabled = false;
          btnRestartSim.IsEnabled = true;
          progressBar.Value = _numSteps;
        }
        _isRunning = false;
      });
    }
  }
}