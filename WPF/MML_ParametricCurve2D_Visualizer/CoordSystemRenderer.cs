using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Configuration class for coordinate system visual styling.
  /// </summary>
  /// <remarks>
  /// This class provides comprehensive control over the appearance of the coordinate system,
  /// including colors, line thicknesses, fonts, and visibility options for various elements.
  /// </remarks>
  public class CoordSystemStyle
  {
    /// <summary>Gets or sets the brush used for the main axis lines.</summary>
    public Brush AxisColor { get; set; } = Brushes.Black;

    /// <summary>Gets or sets the brush used for grid lines.</summary>
    public Brush GridColor { get; set; } = Brushes.LightGray;

    /// <summary>Gets or sets the brush used for tick marks.</summary>
    public Brush TickColor { get; set; } = Brushes.Black;

    /// <summary>Gets or sets the brush used for axis labels.</summary>
    public Brush LabelColor { get; set; } = Brushes.Black;

    /// <summary>Gets or sets the thickness of axis lines in pixels.</summary>
    public double AxisThickness { get; set; } = 1.5;

    /// <summary>Gets or sets the thickness of grid lines in pixels.</summary>
    public double GridThickness { get; set; } = 0.5;

    /// <summary>Gets or sets the length of tick marks in pixels.</summary>
    public double TickLength { get; set; } = 5;

    /// <summary>Gets or sets the font size for axis labels.</summary>
    public double LabelFontSize { get; set; } = 11;

    /// <summary>Gets or sets the font family for axis labels.</summary>
    public FontFamily LabelFontFamily { get; set; } = new FontFamily("Segoe UI");

    /// <summary>Gets or sets whether to display the grid lines.</summary>
    public bool ShowGrid { get; set; } = true;

    /// <summary>Gets or sets whether to display numerical labels on the axes.</summary>
    public bool ShowAxisLabels { get; set; } = true;

    /// <summary>Gets or sets the horizontal offset for Y-axis labels from the axis.</summary>
    public double LabelOffsetX { get; set; } = 5;

    /// <summary>Gets or sets the vertical offset for X-axis labels from the axis.</summary>
    public double LabelOffsetY { get; set; } = 5;
  }

  /// <summary>
  /// Renders a complete coordinate system with axes, grid lines, ticks, and labels.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class is responsible for drawing all the visual elements of a 2D coordinate system:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Grid lines at calculated tick positions</description></item>
  ///   <item><description>X and Y axes (at y=0 and x=0, or at edges if 0 is out of range)</description></item>
  ///   <item><description>Tick marks at nice rounded intervals</description></item>
  ///   <item><description>Numerical labels with appropriate formatting</description></item>
  /// </list>
  /// <para>
  /// The renderer works with the <see cref="AxisTickCalculator"/> to determine optimal
  /// tick positions and the <see cref="CoordTransform"/> to convert between coordinate systems.
  /// </para>
  /// </remarks>
  public static class CoordSystemRenderer
  {
    private static readonly CoordSystemStyle DefaultStyle = new CoordSystemStyle();

    /// <summary>
    /// Draws a complete coordinate system on the specified canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    /// <param name="dataXMin">The minimum X value from the data.</param>
    /// <param name="dataXMax">The maximum X value from the data.</param>
    /// <param name="dataYMin">The minimum Y value from the data.</param>
    /// <param name="dataYMax">The maximum Y value from the data.</param>
    /// <param name="style">Optional styling configuration. Uses default if null.</param>
    /// <remarks>
    /// The coordinate system is drawn in layers: grid (back), axes (middle), ticks and labels (front).
    /// </remarks>
    public static void Draw(Canvas canvas, CoordSystemParams coordParams,
      double dataXMin, double dataXMax, double dataYMin, double dataYMax,
      CoordSystemStyle? style = null)
    {
      style ??= DefaultStyle;

      var (xTickInfo, yTickInfo) = AxisTickCalculator.CalculateAxisTicks(
        dataXMin, dataXMax, dataYMin, dataYMax, 10, 8);

      if (style.ShowGrid)
      {
        DrawGrid(canvas, coordParams, xTickInfo, yTickInfo, style);
      }

      DrawAxes(canvas, coordParams, xTickInfo, yTickInfo, style);
      DrawXAxisTicks(canvas, coordParams, xTickInfo, yTickInfo, style);
      DrawYAxisTicks(canvas, coordParams, xTickInfo, yTickInfo, style);
    }

    /// <summary>
    /// Draws vertical and horizontal grid lines at tick positions.
    /// </summary>
    private static void DrawGrid(Canvas canvas, CoordSystemParams coordParams,
      AxisTickInfo xTicks, AxisTickInfo yTicks, CoordSystemStyle style)
    {
      foreach (var tick in xTicks.Ticks)
      {
        Point top = CoordTransform.WorldToScreen(tick.Value, yTicks.Max, coordParams);
        Point bottom = CoordTransform.WorldToScreen(tick.Value, yTicks.Min, coordParams);

        Line gridLine = new Line
        {
          X1 = top.X, Y1 = top.Y,
          X2 = bottom.X, Y2 = bottom.Y,
          Stroke = style.GridColor,
          StrokeThickness = style.GridThickness
        };
        canvas.Children.Add(gridLine);
      }

      foreach (var tick in yTicks.Ticks)
      {
        Point left = CoordTransform.WorldToScreen(xTicks.Min, tick.Value, coordParams);
        Point right = CoordTransform.WorldToScreen(xTicks.Max, tick.Value, coordParams);

        Line gridLine = new Line
        {
          X1 = left.X, Y1 = left.Y,
          X2 = right.X, Y2 = right.Y,
          Stroke = style.GridColor,
          StrokeThickness = style.GridThickness
        };
        canvas.Children.Add(gridLine);
      }
    }

    /// <summary>
    /// Draws the X and Y axes.
    /// </summary>
    /// <remarks>
    /// Axes are drawn at y=0 and x=0 if these values are within the data range.
    /// Otherwise, they are drawn at the edge of the visible area.
    /// </remarks>
    private static void DrawAxes(Canvas canvas, CoordSystemParams coordParams,
      AxisTickInfo xTicks, AxisTickInfo yTicks, CoordSystemStyle style)
    {
      double xAxisY = 0;
      if (yTicks.Min > 0) xAxisY = yTicks.Min;
      else if (yTicks.Max < 0) xAxisY = yTicks.Max;

      double yAxisX = 0;
      if (xTicks.Min > 0) yAxisX = xTicks.Min;
      else if (xTicks.Max < 0) yAxisX = xTicks.Max;

      Point xAxisStart = CoordTransform.WorldToScreen(xTicks.Min, xAxisY, coordParams);
      Point xAxisEnd = CoordTransform.WorldToScreen(xTicks.Max, xAxisY, coordParams);

      Line xAxis = new Line
      {
        X1 = xAxisStart.X, Y1 = xAxisStart.Y,
        X2 = xAxisEnd.X, Y2 = xAxisEnd.Y,
        Stroke = style.AxisColor,
        StrokeThickness = style.AxisThickness
      };
      canvas.Children.Add(xAxis);

      Point yAxisStart = CoordTransform.WorldToScreen(yAxisX, yTicks.Min, coordParams);
      Point yAxisEnd = CoordTransform.WorldToScreen(yAxisX, yTicks.Max, coordParams);

      Line yAxis = new Line
      {
        X1 = yAxisStart.X, Y1 = yAxisStart.Y,
        X2 = yAxisEnd.X, Y2 = yAxisEnd.Y,
        Stroke = style.AxisColor,
        StrokeThickness = style.AxisThickness
      };
      canvas.Children.Add(yAxis);
    }

    /// <summary>
    /// Draws tick marks and labels along the X axis.
    /// </summary>
    private static void DrawXAxisTicks(Canvas canvas, CoordSystemParams coordParams,
      AxisTickInfo xTicks, AxisTickInfo yTicks, CoordSystemStyle style)
    {
      double tickY = 0;
      if (yTicks.Min > 0) tickY = yTicks.Min;
      else if (yTicks.Max < 0) tickY = yTicks.Max;

      bool labelsBelow = tickY >= 0 || yTicks.Max < 0;

      foreach (var tick in xTicks.Ticks)
      {
        Point tickPos = CoordTransform.WorldToScreen(tick.Value, tickY, coordParams);

        Line tickLine = new Line
        {
          X1 = tickPos.X, Y1 = tickPos.Y - style.TickLength / 2,
          X2 = tickPos.X, Y2 = tickPos.Y + style.TickLength / 2,
          Stroke = style.TickColor,
          StrokeThickness = 1
        };
        canvas.Children.Add(tickLine);

        if (style.ShowAxisLabels)
        {
          TextBlock label = new TextBlock
          {
            Text = tick.Label,
            Foreground = style.LabelColor,
            FontSize = style.LabelFontSize,
            FontFamily = style.LabelFontFamily
          };

          label.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
          double labelWidth = label.DesiredSize.Width;
          double labelHeight = label.DesiredSize.Height;

          double labelX = tickPos.X - labelWidth / 2;
          double labelY = labelsBelow ? tickPos.Y + style.LabelOffsetY : tickPos.Y - labelHeight - style.LabelOffsetY;

          canvas.Children.Add(label);
          Canvas.SetLeft(label, labelX);
          Canvas.SetTop(label, labelY);
        }
      }
    }

    /// <summary>
    /// Draws tick marks and labels along the Y axis.
    /// </summary>
    private static void DrawYAxisTicks(Canvas canvas, CoordSystemParams coordParams,
      AxisTickInfo xTicks, AxisTickInfo yTicks, CoordSystemStyle style)
    {
      double tickX = 0;
      if (xTicks.Min > 0) tickX = xTicks.Min;
      else if (xTicks.Max < 0) tickX = xTicks.Max;

      bool labelsLeft = tickX <= 0 || xTicks.Min > 0;

      foreach (var tick in yTicks.Ticks)
      {
        Point tickPos = CoordTransform.WorldToScreen(tickX, tick.Value, coordParams);

        Line tickLine = new Line
        {
          X1 = tickPos.X - style.TickLength / 2, Y1 = tickPos.Y,
          X2 = tickPos.X + style.TickLength / 2, Y2 = tickPos.Y,
          Stroke = style.TickColor,
          StrokeThickness = 1
        };
        canvas.Children.Add(tickLine);

        if (style.ShowAxisLabels)
        {
          TextBlock label = new TextBlock
          {
            Text = tick.Label,
            Foreground = style.LabelColor,
            FontSize = style.LabelFontSize,
            FontFamily = style.LabelFontFamily
          };

          label.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
          double labelWidth = label.DesiredSize.Width;
          double labelHeight = label.DesiredSize.Height;

          double labelX = labelsLeft ? tickPos.X - labelWidth - style.LabelOffsetX : tickPos.X + style.LabelOffsetX;
          double labelY = tickPos.Y - labelHeight / 2;

          canvas.Children.Add(label);
          Canvas.SetLeft(label, labelX);
          Canvas.SetTop(label, labelY);
        }
      }
    }

    /// <summary>
    /// Updates coordinate system parameters with nice rounded bounds and calculates scales.
    /// </summary>
    /// <param name="coordParams">The parameters object to update.</param>
    /// <param name="dataXMin">The minimum X value from the data.</param>
    /// <param name="dataXMax">The maximum X value from the data.</param>
    /// <param name="dataYMin">The minimum Y value from the data.</param>
    /// <param name="dataYMax">The maximum Y value from the data.</param>
    /// <param name="preserveAspectRatio">If true, uses uniform scaling for both axes.</param>
    /// <remarks>
    /// <para>
    /// This method performs several calculations:
    /// </para>
    /// <list type="number">
    ///   <item><description>Computes nice rounded bounds using <see cref="AxisTickCalculator"/></description></item>
    ///   <item><description>Calculates X and Y scale factors to fit the canvas (with 10% margin)</description></item>
    ///   <item><description>Computes center offsets to center the visualization</description></item>
    /// </list>
    /// <para>
    /// When <paramref name="preserveAspectRatio"/> is true, both axes use the same scale factor
    /// (the smaller of the two), ensuring that circles appear as circles and geometric shapes
    /// maintain their true proportions. This is particularly important for parametric curves
    /// like spirals, Lissajous figures, and other curves where shape matters.
    /// </para>
    /// </remarks>
    public static void UpdateParamsWithNiceBounds(CoordSystemParams coordParams,
      double dataXMin, double dataXMax, double dataYMin, double dataYMax,
      bool preserveAspectRatio = false)
    {
      var (xTicks, yTicks) = AxisTickCalculator.CalculateAxisTicks(
        dataXMin, dataXMax, dataYMin, dataYMax);

      coordParams._xMin = xTicks.Min;
      coordParams._xMax = xTicks.Max;
      coordParams._yMin = yTicks.Min;
      coordParams._yMax = yTicks.Max;

      double xRange = coordParams._xMax - coordParams._xMin;
      double yRange = coordParams._yMax - coordParams._yMin;

      double midPointX = (coordParams._xMin + coordParams._xMax) / 2;
      double midPointY = (coordParams._yMin + coordParams._yMax) / 2;

      double availableWidth = coordParams._windowWidth * 0.9;
      double availableHeight = coordParams._windowHeight * 0.9;

      if (preserveAspectRatio)
      {
        double scaleX = availableWidth / xRange;
        double scaleY = availableHeight / yRange;
        double uniformScale = Math.Min(scaleX, scaleY);

        coordParams._scaleX = uniformScale;
        coordParams._scaleY = uniformScale;
      }
      else
      {
        coordParams._scaleX = availableWidth / xRange;
        coordParams._scaleY = availableHeight / yRange;
      }

      coordParams._centerX = coordParams._windowWidth / 2 - midPointX * coordParams._scaleX;
      coordParams._centerY = coordParams._windowHeight / 2 + midPointY * coordParams._scaleY;
    }
  }
}
