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
  class MultiLoadedFunction : LoadedFunction
  {
    public MML.Vector _multiFuncX;
    public MML.Matrix _multiFuncY;

    public override double GetMinX()
    {
      return _multiFuncX.Elements.Min();
    }

    public override double GetMaxX()
    {
      return _multiFuncX.Elements.Max();
    }

    public override double GetMinY()
    {
      double min = _multiFuncY.ElemAt(0, 0);
      for (int j = 0; j < _multiFuncY.Rows; j++)
        for (int i = 0; i < _multiFuncY.Cols; i++)
          if (_multiFuncY.ElemAt(j, i) < min)
            min = _multiFuncY.ElemAt(j, i);

      return min;
    }
    public override double GetMaxY()
    {
      double max = _multiFuncY.ElemAt(0, 0);
      for (int j = 0; j < _multiFuncY.Rows; j++)
      for (int i = 0; i < _multiFuncY.Cols; i++)
        if (_multiFuncY.ElemAt(j, i) > max)
          max = _multiFuncY.ElemAt(j, i);

      return max;
    }
    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      Utils.DrawCoordSystem(mainCanvas, coordSysParams, GetMinX(), GetMaxX(), GetMinY(), GetMaxY());
//      Utils.DrawCoordSystem(mainCanvas, coordSysParams, coordSysParams._xMin, coordSysParams._xMax, coordSysParams._yMin, coordSysParams._yMax);

      List<Color> colors = new List<Color>();
      colors.Add(Colors.Yellow);
      colors.Add(Colors.DeepSkyBlue);
      colors.Add(Colors.Red);
      colors.Add(Colors.DarkOrange);
      colors.Add(Colors.Yellow);

      List<Brush> brushes= new List<Brush>();
      brushes.Add(Brushes.Yellow);
      brushes.Add(Brushes.DeepSkyBlue);
      brushes.Add(Brushes.Red);
      brushes.Add(Brushes.DarkOrange);
      brushes.Add(Brushes.Yellow);

      for (int i = 0; i < _multiFuncX.Elements.Length; i++)
      {
        // idemo po pojedinacnim funkcijama, odnosno njihovim redovim u matrici
        //for (int j = 0; j < _multiFuncY.Rows; j++)
        //  Utils.DrawPoint(mainCanvas, coordSysParams, _multiFuncX.Elements[i], _multiFuncY.ElemAt(j, i), colors[j]);
      }

      for (int i = 0; i < _multiFuncX.Elements.Length-1; i++)
      {
        // idemo po pojedinacnim funkcijama, odnosno njihovim redovim u matrici
        for (int j = 0; j < _multiFuncY.Rows; j++)
        {
          Line xAxis = new Line();
          xAxis.Stroke = brushes[j];

          double x1 = coordSysParams._centerX + _multiFuncX.Elements[i] * coordSysParams._scaleX;
          double y1 = coordSysParams._centerY - _multiFuncY.ElemAt(j, i) * coordSysParams._scaleY;
          double x2 = coordSysParams._centerX + _multiFuncX.Elements[i + 1] * coordSysParams._scaleX;
          double y2 = coordSysParams._centerY - _multiFuncY.ElemAt(j, i+1) * coordSysParams._scaleY;

          xAxis.X1 = x1;
          xAxis.Y1 = y1;
          xAxis.X2 = x2;
          xAxis.Y2 = y2;

          mainCanvas.Children.Add(xAxis);
        }
      }
    }
  };
}
