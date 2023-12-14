using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;

namespace MML_RealFunctionVisualizer
{
  class SingleLoadedFunction : LoadedFunction
  {
    public LoadedType _loadedType;
    public List<double> _xVals = new List<double>();
    public List<double> _yVals = new List<double>();

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

      for (int i = 0; i < _xVals.Count; i++)
      {

        Utils.DrawCoordSystem(mainCanvas, coordSysParams, coordSysParams._xMin, coordSysParams._xMax, coordSysParams._yMin, coordSysParams._yMax);

        Utils.DrawPoint(mainCanvas, coordSysParams, _xVals[i], _yVals[i], Colors.Blue);

        //Rectangle rect = new Rectangle();
        //rect.Width = 100;
        //rect.Height = 100;
        //rect.Fill = new SolidColorBrush(Colors.PaleVioletRed);
        //mainCanvas.Children.Add(rect);
        //Canvas.SetLeft(rect, 10);
        //Canvas.SetTop(rect, 10);
      }
    }
  };
}
