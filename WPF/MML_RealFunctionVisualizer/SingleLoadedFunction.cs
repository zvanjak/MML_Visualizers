using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  class SingleLoadedFunction : LoadedFunction
  {
    public string _title = "";
    public LoadedType _loadedType;
    public List<double> _xVals = new List<double>();
    public List<double> _yVals = new List<double>();
    private int _index;

    public SingleLoadedFunction(int inIndex)
    {
      _index = inIndex;
    }

    public override int GetNumPoints()
    {
      return _xVals.Count;
    }
    public override double GetMinX()
    {
      return _xVals.Min();
    }

    public override double GetMaxX()
    {
      return _xVals.Max();
    }

    public override double GetMinY()
    {
      return _yVals.Min();
    }
    public override double GetMaxY()
    {
      return _yVals.Max();
    }

    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      //List<Color> colors = new List<Color>();
      //colors.Add(Colors.Black);
      //colors.Add(Colors.Blue);
      //colors.Add(Colors.Red);
      //colors.Add(Colors.Green);
      //colors.Add(Colors.Orange);

      List<Brush> brushes = new List<Brush>();
      brushes.Add(Brushes.Black);
      brushes.Add(Brushes.Blue);
      brushes.Add(Brushes.Red);
      brushes.Add(Brushes.Green);
      brushes.Add(Brushes.Orange);
      brushes.Add(Brushes.Purple);
      brushes.Add(Brushes.Brown);
      brushes.Add(Brushes.Cyan);
      brushes.Add(Brushes.Magenta);
      brushes.Add(Brushes.Gray);

      Utils.DrawCoordSystem(mainCanvas, coordSysParams, coordSysParams._xMin, coordSysParams._xMax, coordSysParams._yMin, coordSysParams._yMax);

      for (int i = 0; i < _xVals.Count; i++)
      {
        //Utils.DrawPoint(mainCanvas, coordSysParams, _xVals[i], _yVals[i], colors[_index]);

        //Rectangle rect = new Rectangle();
        //rect.Width = 100;
        //rect.Height = 100;
        //rect.Fill = new SolidColorBrush(Colors.PaleVioletRed);
        //mainCanvas.Children.Add(rect);
        //Canvas.SetLeft(rect, 10);
        //Canvas.SetTop(rect, 10);
      }

      for (int i = 0; i < _xVals.Count - 1; i++)
      {
        Line xAxis = new Line();
        if (_index < brushes.Count)
          xAxis.Stroke = brushes[_index];
        else
          xAxis.Stroke = Brushes.Black;

        double x1 = coordSysParams._centerX + _xVals[i] * coordSysParams._scaleX;
        double y1 = coordSysParams._centerY - _yVals[i] * coordSysParams._scaleY;
        double x2 = coordSysParams._centerX + _xVals[i + 1] * coordSysParams._scaleX;
        double y2 = coordSysParams._centerY - _yVals[i + 1] * coordSysParams._scaleY;

        xAxis.X1 = x1;
        xAxis.Y1 = y1;
        xAxis.X2 = x2;
        xAxis.Y2 = y2;

        mainCanvas.Children.Add(xAxis);
      }
    }
  };
}
