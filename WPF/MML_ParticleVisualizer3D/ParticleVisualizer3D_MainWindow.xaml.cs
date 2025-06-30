using System;
using System.Collections.Generic;
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
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;

using MML;
using WPF3DHelperLib;

namespace MML_ParticleVisualizer3D
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class ParticleVisualizer3D_MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    Point3D _cameraPoint = new Point3D(1350, 1100, 1350);
    double _axisWidth = 0.5;
    double _axisLen = 500;
    double _boxLen = 1000;
    double _lineWidth = 0.25;

    double _width = 1000;
    double _height = 1000;
    double _depth = 1000;

    bool _bShowBox = true;

    int _numSteps = 0;
    int _stepDelayMiliSec = 10;

    int _currStep = 0;
    bool _isPausedClicked = false;

    List<ParticleData3D> _balls = new List<ParticleData3D>();

    Model3DGroup _myModel3DGroup = new Model3DGroup();

    public ParticleVisualizer3D_MainWindow()
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
        MessageBox.Show("Error loading data from file.");
        return;
      }

      txtNumSteps.Text = _numSteps.ToString();
      txtDT.Text = _stepDelayMiliSec.ToString();

      AddObjectsToScene();
    }

    private int LoadIntParamFromLine(string line, string name)
    {
      string[] parts = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
      if (parts.Length != 2)
        throw new Exception("Invalid " + name + " in line: " + line);

      return int.Parse(parts[1]);
    }

    private double LoadRealParamFromLine(string line, string name)
    {
      string[] parts = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
      if (parts.Length != 2)
        throw new Exception("Invalid " + name + " in line: " + line);

      return double.Parse(parts[1]);
    }

    public bool LoadData(string fileName)
    {
      if (File.Exists(fileName) == false)
      {
        MessageBox.Show("File does not exist: " + fileName);
        return false;
      }

      // get number of balls
      int numBalls = 0;
      string[] lines = System.IO.File.ReadAllLines(fileName);

      if (lines[0] == "PARTICLE_SIMULATION_DATA_3D")
      {
        try
        {
          _width = LoadRealParamFromLine (lines[1], "width");

          // read width and height
          string[] parts1 = lines[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts1.Length != 2)
            throw new Exception("Invalid width!");

          _width = int.Parse(parts1[1]);

          string[] parts2 = lines[2].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts2.Length != 2)
            throw new Exception("Invalid height!");

          _height = int.Parse(parts2[1]);

          string[] parts3 = lines[3].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts3.Length != 2)
            throw new Exception("Invalid depth!");

          _depth = int.Parse(parts3[1]);

          // read the number of balls
          string[] parts = lines[4].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

          if (parts.Length != 2)
            throw new Exception("Invalid number of balls in " + fileName);

          numBalls = int.Parse(parts[1]);

          int lineNumber = 5;

          // read the balls attributes - name, radius, type
          for (int i = 0; i < numBalls; i++)
          {
            parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts.Length != 3)
              throw new Exception("Invalid ball attributes in SimData.txt");

            string name = parts[0];
            string color = parts[1];
            double radius = double.Parse(parts[2], CultureInfo.InvariantCulture);

            MeshGeometry3D sphereGeometry = Geometries.CreateSphere(new Point3D(0, 0, 0), radius);
            Color ballColor = (Color)ColorConverter.ConvertFromString(color);
            var sphereMaterial = new DiffuseMaterial(new SolidColorBrush(ballColor));

            GeometryModel3D sphereModel = new GeometryModel3D(sphereGeometry, sphereMaterial);

            // now, let's create particle data and mesh geometry for the ball
            ParticleData3D ball = new ParticleData3D(name, color, radius, sphereModel);

            //ball._geomModel = sphereModel;

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

            if (parts[0] != "Step")
              throw new Exception("Invalid step in SimData.txt");

            if (parts[1] != i.ToString())
              throw new Exception("Invalid step number in SimData.txt");

            double T = double.Parse(parts[2], CultureInfo.InvariantCulture);

            // read the positions of the balls
            for (int j = 0; j < numBalls; j++)
            {
              parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
              if (parts.Length != 4)
                throw new Exception("Invalid ball position coords in SimData.txt, line number - " + lineNumber.ToString());

              int index = int.Parse(parts[0]);
              double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
              double y = double.Parse(parts[2], CultureInfo.InvariantCulture);
              double z = double.Parse(parts[3], CultureInfo.InvariantCulture);

              Vector3Cartesian pos = new Vector3Cartesian(x, y, z);

              _balls[j].AddPos(pos);
            }
          }

          // now initialize balls position n the scene to the first step
          for (int i = 0; i < _balls.Count; i++)
          {
            TranslateTransform3D Off = new TranslateTransform3D();
            Off.OffsetX = _balls[i].Pos(0).X;
            Off.OffsetY = _balls[i].Pos(0).Y;
            Off.OffsetZ = _balls[i].Pos(0).Z;
            _balls[i]._geomModel.Transform = Off;
          }
        }
        catch (Exception ex)
        {
          MessageBox.Show("Error - " + ex.Message);
          return false;
        }
      }
      else
      {
        MessageBox.Show("Invalid file format. Expected 'PARTICLE_SIMULATION_DATA_3D' at the beginning.");
        return false;
      }

      return true;
    }

    private void AddObjectsToScene()
    {
      _myModel3DGroup.Children.Clear();

      _helper.InitCamera(_cameraPoint);

      //AmbientLight ambLight = new AmbientLight();
      //ambLight.Color = Colors.White;
      //_myModel3DGroup.Children.Add(ambLight);

      DirectionalLight myDirectionalLight1 = new DirectionalLight(diffuseColor: Colors.White, direction: new Vector3D(-0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight1);

      DirectionalLight myDirectionalLight2 = new DirectionalLight(diffuseColor: Colors.White, direction: new Vector3D(0.31, 0.2, -0.61));
      _myModel3DGroup.Children.Add(myDirectionalLight2);

      myViewport3D.Camera = _helper._myCamera;


      for (int i = 0; i < _balls.Count; i++)
      {
        _myModel3DGroup.Children.Add(_balls[i]._geomModel);
      }

      ModelVisual3D myModelVisual3D = new ModelVisual3D { Content = _myModel3DGroup };

      myViewport3D.Children.Add(myModelVisual3D);

      if (_bShowBox == true)
      {
        MeshGeometry3D xyPlaneMesh = Geometries.CreateParallelepiped(new Point3D(_boxLen / 2, _boxLen / 2, 0), _boxLen, _boxLen, 0.02);
        //var xyPlaneMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightSkyBlue));
        var xyPlaneMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightSkyBlue) { Opacity = 0.3 });
        GeometryModel3D xyPlaneModel = new GeometryModel3D(xyPlaneMesh, xyPlaneMaterial);

        MeshGeometry3D xzPlaneMesh = Geometries.CreateParallelepiped(new Point3D(_boxLen / 2, 0, _boxLen / 2), _boxLen, 0.02, _boxLen);
        var xzPlaneMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightSkyBlue) { Opacity = 0.3 });
        GeometryModel3D xzPlaneModel = new GeometryModel3D(xzPlaneMesh, xzPlaneMaterial);

        MeshGeometry3D yzPlaneMesh = Geometries.CreateParallelepiped(new Point3D(0, _boxLen / 2, _boxLen / 2), 0.02, _boxLen, _boxLen);
        var yzPlaneMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.LightSkyBlue) { Opacity = 0.3 });
        GeometryModel3D yzPlaneModel = new GeometryModel3D(yzPlaneMesh, yzPlaneMaterial);

        _myModel3DGroup.Children.Add(xyPlaneModel);
        _myModel3DGroup.Children.Add(xzPlaneModel);
        _myModel3DGroup.Children.Add(yzPlaneModel);
      }
      else
        Utils.DrawCoordSystem(_myModel3DGroup, _lineWidth * 3, _axisLen);

      // adding the lines
      //for (int i = 0; i < _balls.Count; i++)
      //{
      //  for (int j = 0; j < _numSteps - 1; j++)
      //  {
      //    MeshGeometry3D line = Geometries.CreateSimpleLine(_balls[i].Pos(j), _balls[i].Pos(j + 1), _lineWidth, _lineWidth * 2);
      //    var lineMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.OrangeRed));
      //    GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);
      //    _myModel3DGroup.Children.Add(lineModel);
      //  }
      //}
    }

    private void btnStartSim_Click(object sender, RoutedEventArgs e)
    {
      btnRestartSim.IsEnabled = false;
      btnStartSim.IsEnabled = false;
      btnPauseSim.IsEnabled = true;

      _stepDelayMiliSec = int.Parse(txtDT.Text);

      int refreshEvery = 1; //  Convert.ToInt16(txtRefreshEvery.Text);

      // run animations in a separate thread
      Task.Run(() =>
      {
        Animate(_numSteps, refreshEvery);
      });
    }

    private void SetBallsPosToTimestep(int timeStep)
    {
      if (timeStep > 0 && timeStep < _balls.Count)
      {
        for (int i = 0; i < _balls.Count; i++)
        {
          TranslateTransform3D Off = new TranslateTransform3D();
          Off.OffsetX = _balls[i].Pos(timeStep).X;
          Off.OffsetY = _balls[i].Pos(timeStep).Y;
          Off.OffsetZ = _balls[i].Pos(timeStep).Z;

          _balls[i]._geomModel.Transform = Off;
        }
      }
    }

    private void Animate(int numSteps, int refreshEvery)
    {
      for (int t = _currStep; t < numSteps; t++, _currStep++)
      {
        Thread.Sleep(_stepDelayMiliSec);

        if (_isPausedClicked)
        {
          _isPausedClicked = false;
          return;
        }

        //if (t > 0)
        //{
        //  MeshGeometry3D line = Geometries.CreateSimpleLine(_curveTrace[t - 1], _curveTrace[t], 10, 5);

        //  var lineMaterial = new DiffuseMaterial(new SolidColorBrush(Colors.OrangeRed));
        //  GeometryModel3D lineModel = new GeometryModel3D(line, lineMaterial);

        //  _myModel3DGroup.Children.Add(lineModel);
        //}

        if (t % refreshEvery == 0)
        {
          this.Dispatcher.Invoke((Action)(() =>
          {
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

    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonDown(e.GetPosition(this));

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseLeftButtonUp();

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonDown(e.GetPosition(this));

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _helper.Window_MouseRightButtonUp();

      _cameraPoint = _helper._cameraPos;
    }
    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      _helper.Window_MouseMove(myViewport3D, e.GetPosition(this), sender, e);

      _cameraPoint = _helper._cameraPos;
    }
    public void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      _helper.Window_MouseWheel(myViewport3D, sender, e);

      _cameraPoint = _helper._cameraPos;
    }

    private void btnIncWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 1.1;
      AddObjectsToScene();
    }

    private void btnDecWidth_Click(object sender, RoutedEventArgs e)
    {
      _lineWidth *= 0.9;
      AddObjectsToScene();
    }

    private void btnLookAtCenter_Click(object sender, RoutedEventArgs e)
    {
      _helper.InitLookAtPoint(new Point3D(_boxLen / 2, _boxLen / 2, _boxLen / 2));
    }

  }
}