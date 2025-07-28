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
      Utils.DrawCoordSystem(mainCanvas, coordSysParams, GetMinX(), GetMaxX(), GetMinY(), GetMaxY());

      for (int i = 0; i < _xVals.Count; i++)
      {
        //Utils.DrawPoint(mainCanvas, coordSysParams, _xVals[i], _yVals[i], LineColors.colors[_index]);

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
        if (_index < LineBrushes.brushes.Length )
          xAxis.Stroke = LineBrushes.brushes[_index];
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
