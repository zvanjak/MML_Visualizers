using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Configuration for coordinate system rendering.
  /// </summary>
  public class CoordSystemStyle
  {
    public Brush AxisColor { get; set; } = Brushes.Black;
    public Brush GridColor { get; set; } = Brushes.LightGray;
    public Brush TickColor { get; set; } = Brushes.Black;
    public Brush LabelColor { get; set; } = Brushes.Black;

    public double AxisThickness { get; set; } = 1.5;
    public double GridThickness { get; set; } = 0.5;
    public double TickLength { get; set; } = 5;

    public double LabelFontSize { get; set; } = 11;
    public FontFamily LabelFontFamily { get; set; } = new FontFamily("Segoe UI");

    public bool ShowGrid { get; set; } = true;
    public bool ShowAxisLabels { get; set; } = true;

    public double LabelOffsetX { get; set; } = 5;
    public double LabelOffsetY { get; set; } = 5;
  }

  /// <summary>
  /// Renders coordinate system with properly rounded axis ticks.
  /// </summary>
  public static class CoordSystemRenderer
  {
    private static readonly CoordSystemStyle DefaultStyle = new CoordSystemStyle();

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
