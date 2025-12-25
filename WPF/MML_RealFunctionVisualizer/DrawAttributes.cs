using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace MML_RealFunctionVisualizer
{
  public enum LineStyle
  {
    Solid,
    Dashed,
    Dotted,
    DashDot
  }

  public class FunctionDrawStyle
  {
    public Brush Stroke { get; set; } = Brushes.Black;
    public double StrokeThickness { get; set; } = 1.5;
    public LineStyle LineStyle { get; set; } = LineStyle.Solid;
    public bool ShowPoints { get; set; } = false;
    public double PointRadius { get; set; } = 3;

    public DoubleCollection GetDashArray()
    {
      return LineStyle switch
      {
        LineStyle.Dashed => new DoubleCollection { 4, 2 },
        LineStyle.Dotted => new DoubleCollection { 1, 2 },
        LineStyle.DashDot => new DoubleCollection { 4, 2, 1, 2 },
        _ => new DoubleCollection()
      };
    }

    public static FunctionDrawStyle CreateDefault(int index)
    {
      return new FunctionDrawStyle
      {
        Stroke = LineBrushes.GetBrush(index),
        StrokeThickness = 1.5,
        LineStyle = LineStyle.Solid
      };
    }
  }

  // Contains Color instances for drawing lines
  public static class LineColors
  {
    public static readonly Color[] Colors = new Color[]
    {
      System.Windows.Media.Colors.Black,
      System.Windows.Media.Colors.Blue,
      System.Windows.Media.Colors.Red,
      System.Windows.Media.Colors.Green,
      System.Windows.Media.Colors.Orange,
      System.Windows.Media.Colors.Purple,
      System.Windows.Media.Colors.Brown,
      System.Windows.Media.Colors.Cyan,
      System.Windows.Media.Colors.Magenta,
      System.Windows.Media.Colors.Gray
    };

    public static Color GetColor(int index) => Colors[index % Colors.Length];
  }

  // Contains Brush instances for drawing lines
  public static class LineBrushes
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

    public static Brush GetBrush(int index) => brushes[index % brushes.Length];
  }
}
