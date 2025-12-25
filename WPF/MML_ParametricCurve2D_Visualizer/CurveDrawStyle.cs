using System;
using System.Collections.Generic;
using System.Windows.Media;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Specifies the visual style of a curve line.
  /// </summary>
  public enum LineStyle
  {
    /// <summary>A continuous solid line.</summary>
    Solid,
    /// <summary>A line with regular dashes (4-2 pattern).</summary>
    Dashed,
    /// <summary>A line with small dots (1-2 pattern).</summary>
    Dotted,
    /// <summary>A line alternating between dashes and dots (4-2-1-2 pattern).</summary>
    DashDot
  }

  /// <summary>
  /// Encapsulates all visual styling properties for rendering a parametric curve.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class provides centralized control over the appearance of parametric curves,
  /// including stroke color, line thickness, line pattern, and optional data point markers.
  /// </para>
  /// <para>
  /// Parametric curves typically use a slightly thicker stroke (2.0) compared to function
  /// plots to better show the curve path, especially for complex curves like spirals or Lissajous figures.
  /// </para>
  /// </remarks>
  public class CurveDrawStyle
  {
    /// <summary>
    /// Gets or sets the brush used for the curve's stroke color.
    /// </summary>
    public Brush Stroke { get; set; } = Brushes.Black;

    /// <summary>
    /// Gets or sets the thickness of the curve line in device-independent pixels.
    /// </summary>
    public double StrokeThickness { get; set; } = 2.0;

    /// <summary>
    /// Gets or sets the line pattern style (solid, dashed, dotted, etc.).
    /// </summary>
    public LineStyle LineStyle { get; set; } = LineStyle.Solid;

    /// <summary>
    /// Gets or sets whether to draw circular markers at each sampled point.
    /// </summary>
    public bool ShowPoints { get; set; } = false;

    /// <summary>
    /// Gets or sets the radius of point markers in device-independent pixels.
    /// </summary>
    public double PointRadius { get; set; } = 3;

    /// <summary>
    /// Converts the <see cref="LineStyle"/> to a WPF dash array for stroke rendering.
    /// </summary>
    /// <returns>
    /// A <see cref="DoubleCollection"/> representing the dash pattern,
    /// or an empty collection for solid lines.
    /// </returns>
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

    /// <summary>
    /// Creates a default drawing style with a color selected from the standard palette.
    /// </summary>
    /// <param name="index">The index used to select a color from the palette (wraps around).</param>
    /// <returns>A new <see cref="CurveDrawStyle"/> with default settings and the selected color.</returns>
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
  /// Provides a palette of brushes for drawing parametric curves.
  /// </summary>
  /// <remarks>
  /// Brushes are designed to be visually distinct when multiple curves are displayed
  /// together on the same graph. The palette cycles through colors when more curves
  /// are loaded than available colors.
  /// </remarks>
  public static class CurveBrushes
  {
    /// <summary>
    /// The array of available brushes in the palette.
    /// </summary>
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

    /// <summary>
    /// Gets a brush from the palette by index, with automatic wrap-around.
    /// </summary>
    /// <param name="index">The index of the desired brush.</param>
    /// <returns>The brush at the specified index (modulo palette size).</returns>
    public static Brush GetBrush(int index) => Brushes[index % Brushes.Length];
  }
}
