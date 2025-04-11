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
    List<Vector2Repr> _listVecs = new List<Vector2Repr>();
    CoordSystemParams _coordSystemParams = new CoordSystemParams();
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

      _listVecs = LoadData(fileName);

      InitializeCoordSysParams();

      txtTitle.Text = _title;

      Redraw();
    }
    private void InitializeCoordSysParams()
    {
      _coordSystemParams._xMin = 1e30;
      _coordSystemParams._xMax = -1e30; 
      _coordSystemParams._yMin = 1e30; 
      _coordSystemParams._yMax = -1e30;

      foreach ( var v in _listVecs) {
        if (v.Pos.X < _coordSystemParams._xMin)
          _coordSystemParams._xMin = v.Pos.X;
        if (v.Pos.Y < _coordSystemParams._yMin)
          _coordSystemParams._yMin = v.Pos.Y;
        if (v.Pos.X > _coordSystemParams._xMax)
          _coordSystemParams._xMax = v.Pos.X;
        if (v.Pos.Y > _coordSystemParams._yMax)
          _coordSystemParams._yMax = v.Pos.Y;
      }

      _coordSystemParams._numPoints = _listVecs.Count;

      _coordSystemParams._windowWidth = mainCanvas.Width;
      _coordSystemParams._windowHeight = mainCanvas.Height;

      // izracunati general scale - je li 1, 10, 1000, ili 10-3, 10-6
      // prilagođavanje skaliranja i centra
      // kod prikazivanja teksta, dok je unutar 0.001, 1000, s decimalama
      // inače E notacija
      double midPoint = (_coordSystemParams._xMin + _coordSystemParams._xMax) / 2;
      double midPointY = (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2;
      // ako je 0, onda je tocno sredina
      // ako je manje od 0 , onda je vise sredina prema xMin


      _coordSystemParams._scaleX = _coordSystemParams._windowWidth / (_coordSystemParams._xMax - _coordSystemParams._xMin) * 0.9;
      _coordSystemParams._scaleY = _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) * 0.9;
      _coordSystemParams._centerX = _coordSystemParams._windowWidth / 2 - midPoint * _coordSystemParams._scaleX;
      _coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + midPointY * _coordSystemParams._scaleY;
      //_coordSystemParams._centerY = _coordSystemParams._windowHeight / 2 + (_coordSystemParams._yMin + _coordSystemParams._yMax) / 2 * _coordSystemParams._windowHeight / (_coordSystemParams._yMax - _coordSystemParams._yMin) + _coordSystemParams._windowHeight / 20;

    }
    void Redraw()
    {
      // Clear the canvas
      mainCanvas.Children.Clear();

      for (int i = 0; i < _listVecs.Count; i++)
      {
        double mul = 0.5;

        Line xAxis = new Line();
        xAxis.Stroke = Brushes.Black;

        double x1 = _coordSystemParams._centerX + _listVecs[i].Pos.X * _coordSystemParams._scaleX;
        double y1 = _coordSystemParams._centerY - _listVecs[i].Pos.Y * _coordSystemParams._scaleY;
        double x2 = _coordSystemParams._centerX + (_listVecs[i].Pos.X + mul * _listVecs[i].Vec.X) * _coordSystemParams._scaleX;
        double y2 = _coordSystemParams._centerY - (_listVecs[i].Pos.Y + mul * _listVecs[i].Vec.Y) * _coordSystemParams._scaleY;

        xAxis.X1 = x1;
        xAxis.Y1 = y1;
        xAxis.X2 = x2;
        xAxis.Y2 = y2;

        mainCanvas.Children.Add(xAxis);

        // now let's add arrow on top
        double arrowSize = 10;
        double arrowAngle = 35;
        double angle = Math.Atan2(_listVecs[i].Vec.Y, _listVecs[i].Vec.X);
        double xArrow1 = x2 - arrowSize * Math.Cos(angle + Math.PI * arrowAngle / 180);
        double yArrow1 = y2 + arrowSize * Math.Sin(angle + Math.PI * arrowAngle / 180);
        double xArrow2 = x2 - arrowSize * Math.Cos(angle - Math.PI * arrowAngle / 180);
        double yArrow2 = y2 + arrowSize * Math.Sin(angle - Math.PI * arrowAngle / 180);
        
        Line arrow1 = new Line();
        arrow1.Stroke = Brushes.Black;
        arrow1.X1 = x2;
        arrow1.Y1 = y2;
        arrow1.X2 = xArrow1;
        arrow1.Y2 = yArrow1;
        
        Line arrow2 = new Line();
        arrow2.Stroke = Brushes.Black;
        arrow2.X1 = x2;
        arrow2.Y1 = y2;
        arrow2.X2 = xArrow2;
        arrow2.Y2 = yArrow2;
        mainCanvas.Children.Add(arrow1);
        mainCanvas.Children.Add(arrow2);


      }
      // Draw the coordinate system
      Utils.DrawCoordSystem(mainCanvas, _coordSystemParams, _coordSystemParams._xMin, _coordSystemParams._xMax, _coordSystemParams._yMin, _coordSystemParams._yMax);
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