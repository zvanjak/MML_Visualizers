using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace MML_RealFunctionVisualizer
{
  // containes Color instances for drawing lines
  internal static class LineColors
    {
    public static readonly Color[] colors = new Color[]
    {
      Colors.Black,
      Colors.Blue,
      Colors.Red,
      Colors.Green,
      Colors.Orange,
      Colors.Purple,
      Colors.Brown,
      Colors.Cyan,
      Colors.Magenta,
      Colors.Gray
    };
  }

  // containes Brush instances for drawing lines
  internal static class LineBrushes
  {
    public static readonly Brush[] brushes = new Brush[]
    {
    Brushes.Black,
    Brushes.Blue,
    Brushes.Red,
    Brushes.Green,
    Brushes.Orange,
    Brushes.Purple,
    Brushes.Brown,
    Brushes.Cyan,
    Brushes.Magenta,
    Brushes.Gray,
    Brushes.Yellow
    };

  }
}
