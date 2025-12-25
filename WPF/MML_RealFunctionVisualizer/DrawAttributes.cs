using System;
using System.Windows.Media;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Specifies the visual style of a line.
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
  /// Encapsulates all visual styling properties for rendering a function.
  /// </summary>
  /// <remarks>
  /// This class provides a centralized way to configure the appearance of function curves,
  /// including stroke color, line thickness, line pattern, and optional data point markers.
  /// </remarks>
  public class FunctionDrawStyle
  {
    /// <summary>
    /// Gets or sets the brush used for the function's stroke color.
    /// </summary>
    public Brush Stroke { get; set; } = Brushes.Black;

    /// <summary>
    /// Gets or sets the thickness of the line in device-independent pixels.
    /// </summary>
    public double StrokeThickness { get; set; } = 1.5;

    /// <summary>
    /// Gets or sets the line pattern style (solid, dashed, dotted, etc.).
    /// </summary>
    public LineStyle LineStyle { get; set; } = LineStyle.Solid;

    /// <summary>
    /// Gets or sets whether to draw circular markers at each data point.
    /// </summary>
    public bool ShowPoints { get; set; } = false;

    /// <summary>
    /// Gets or sets the radius of data point markers in device-independent pixels.
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
    /// <returns>A new <see cref="FunctionDrawStyle"/> with default settings and the selected color.</returns>
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

  /// <summary>
  /// Provides a palette of colors for drawing function lines.
  /// </summary>
  /// <remarks>
  /// Colors are designed to be visually distinct and work well on white backgrounds.
  /// </remarks>
  public static class LineColors
  {
    /// <summary>
    /// The array of available colors in the palette.
    /// </summary>
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

    /// <summary>
    /// Gets a color from the palette by index, with automatic wrap-around.
    /// </summary>
    /// <param name="index">The index of the desired color.</param>
    /// <returns>The color at the specified index (modulo palette size).</returns>
    public static Color GetColor(int index) => Colors[index % Colors.Length];
  }

  /// <summary>
  /// Provides a palette of brushes for drawing function lines.
  /// </summary>
  /// <remarks>
  /// Brushes correspond to the colors in <see cref="LineColors"/> and are designed
  /// to be visually distinct when multiple functions are displayed together.
  /// </remarks>
  public static class LineBrushes
  {
    /// <summary>
    /// The array of available brushes in the palette.
    /// </summary>
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

    /// <summary>
    /// Gets a brush from the palette by index, with automatic wrap-around.
    /// </summary>
    /// <param name="index">The index of the desired brush.</param>
    /// <returns>The brush at the specified index (modulo palette size).</returns>
    public static Brush GetBrush(int index) => brushes[index % brushes.Length];
  }
}
