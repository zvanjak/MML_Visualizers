using System;
using System.Collections.Generic;
using System.Windows.Media;

namespace MML_ParametricCurve2D_Visualizer
{
  public enum LineStyle
  {
    Solid,
    Dashed,
    Dotted,
    DashDot
  }

  /// <summary>
  /// Encapsulates drawing style for a curve.
  /// </summary>
  public class CurveDrawStyle
  {
    public Brush Stroke { get; set; } = Brushes.Black;
    public double StrokeThickness { get; set; } = 2.0;
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

    public static CurveDrawStyle CreateDefault(int index)
    {
      return new CurveDrawStyle
      {
        Stroke = CurveBrushes.GetBrush(index),
        StrokeThickness = 2.0,
        LineStyle = LineStyle.Solid
      };
    }
  }

  /// <summary>
  /// Contains Brush instances for drawing curves.
  /// </summary>
  public static class CurveBrushes
  {
    public static readonly Brush[] Brushes = new Brush[]
    {
      System.Windows.Media.Brushes.Black,
      System.Windows.Media.Brushes.Blue,
      System.Windows.Media.Brushes.Red,
      System.Windows.Media.Brushes.Green,
      System.Windows.Media.Brushes.Orange,
      System.Windows.Media.Brushes.Purple,
      System.Windows.Media.Brushes.Brown,
      System.Windows.Media.Brushes.Cyan,
      System.Windows.Media.Brushes.Magenta,
      System.Windows.Media.Brushes.Gray,
      System.Windows.Media.Brushes.Yellow
    };

    public static Brush GetBrush(int index) => Brushes[index % Brushes.Length];
  }
}
