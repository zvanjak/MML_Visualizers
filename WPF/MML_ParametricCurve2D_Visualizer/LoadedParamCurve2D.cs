using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  class LoadedParamCurve2D
  {
    public string _title;
    public int _index = 0;

    public List<double> _tVals = new List<double>();
    public List<double> _xVals = new List<double>();
    public List<double> _yVals = new List<double>();

    public int GetNumPoints()
    {
      return _xVals.Count;
    }
    public double GetMinX()
    {
      return _xVals.Min();
    }

    public double GetMaxX()
    {
      return _xVals.Max();
    }

    public double GetMinY()
    {
      return _yVals.Min();
    }
    public double GetMaxY()
    {
      return _yVals.Max();
    }

    public void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams, Brush drawBrush)
    {
      List<Color> colors = new List<Color>();
      colors.Add(Colors.Black);
      colors.Add(Colors.Blue);
      colors.Add(Colors.Red);
      colors.Add(Colors.Green);
      colors.Add(Colors.Orange);



      Utils.DrawCoordSystem(mainCanvas, coordSysParams, coordSysParams._xMin, coordSysParams._xMax, coordSysParams._yMin, coordSysParams._yMax);

      for (int i = 0; i < _tVals.Count; i++)
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

      for (int i = 0; i < _tVals.Count - 1; i++)
      {
        Line xAxis = new Line();
        xAxis.Stroke = drawBrush;

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
  }
}
