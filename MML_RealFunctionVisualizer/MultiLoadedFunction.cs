using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;

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
      return -15;
    }
    public override double GetMaxY()
    {
      return 5;
    }
    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      Utils.DrawCoordSystem(mainCanvas, coordSysParams, coordSysParams._xMin, coordSysParams._xMax, coordSysParams._yMin, coordSysParams._yMax);

      List<Color> colors = new List<Color>();
      colors.Add(Colors.Blue);
      colors.Add(Colors.Red);
      colors.Add(Colors.Green);
      colors.Add(Colors.Yellow);

      for (int i = 0; i < _multiFuncX.Elements.Length; i++)
      {
        for (int j = 0; j < _multiFuncY.Rows; j++)
          Utils.DrawPoint(mainCanvas, coordSysParams, _multiFuncX.Elements[i], _multiFuncY.ElemAt(j, i), colors[j]);
      }
    }
  };
}
