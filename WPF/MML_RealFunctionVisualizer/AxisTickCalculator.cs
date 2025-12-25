using System;
using System.Collections.Generic;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Represents a single tick mark on a coordinate axis.
  /// </summary>
  public class AxisTick
  {
    /// <summary>
    /// Gets or sets the numerical value at this tick position.
    /// </summary>
    public double Value { get; set; }

    /// <summary>
    /// Gets or sets the formatted text label to display at this tick.
    /// </summary>
    public string Label { get; set; } = "";

    /// <summary>
    /// Gets or sets whether this is a major tick (typically longer with a label).
    /// </summary>
    public bool IsMajor { get; set; } = true;
  }

  /// <summary>
  /// Contains comprehensive tick information for rendering an axis.
  /// </summary>
  /// <remarks>
  /// This class holds all the information needed to render axis ticks,
  /// including the computed nice bounds, tick positions, and formatting settings.
  /// </remarks>
  public class AxisTickInfo
  {
    /// <summary>Gets or sets the nice rounded minimum value for the axis.</summary>
    public double Min { get; set; }

    /// <summary>Gets or sets the nice rounded maximum value for the axis.</summary>
    public double Max { get; set; }

    /// <summary>Gets or sets the spacing between consecutive ticks.</summary>
    public double TickSpacing { get; set; }

    /// <summary>Gets or sets the list of tick marks for the axis.</summary>
    public List<AxisTick> Ticks { get; set; } = new List<AxisTick>();

    /// <summary>Gets or sets the number of decimal places for tick labels.</summary>
    public int DecimalPlaces { get; set; }

    /// <summary>Gets or sets whether tick labels should use scientific notation.</summary>
    public bool UseScientificNotation { get; set; }
  }

  /// <summary>
  /// Calculates optimal axis tick positions with "nice" rounded values.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class implements an algorithm to find visually pleasing tick intervals for axes.
  /// Instead of using arbitrary values, it rounds to "nice" numbers like 1, 2, 2.5, 5, 10
  /// and their powers of 10.
  /// </para>
  /// <para>
  /// The algorithm:
  /// </para>
  /// <list type="number">
  ///   <item><description>Calculate rough tick spacing based on data range and desired tick count</description></item>
  ///   <item><description>Find the magnitude (power of 10) of the spacing</description></item>
  ///   <item><description>Normalize to 1-10 range and find nearest "nice" number</description></item>
  ///   <item><description>Round min/max to tick spacing boundaries</description></item>
  ///   <item><description>Generate tick labels with appropriate formatting</description></item>
  /// </list>
  /// </remarks>
  /// <example>
  /// <code>
  /// // For data ranging from 0.3 to 47.8:
  /// var tickInfo = AxisTickCalculator.CalculateTicks(0.3, 47.8, 8);
  /// // Result: Min=0, Max=50, Ticks at 0, 10, 20, 30, 40, 50
  /// </code>
  /// </example>
  public static class AxisTickCalculator
  {
    /// <summary>
    /// Preferred tick intervals normalized to the 1-10 range.
    /// These values produce visually pleasing axis labels.
    /// </summary>
    private static readonly double[] NiceNumbers = { 1.0, 2.0, 2.5, 5.0, 10.0 };

    /// <summary>
    /// Calculates nice tick values for an axis based on data range.
    /// </summary>
    /// <param name="dataMin">The minimum value in the data.</param>
    /// <param name="dataMax">The maximum value in the data.</param>
    /// <param name="targetTickCount">The approximate number of ticks desired (default: 8).</param>
    /// <returns>An <see cref="AxisTickInfo"/> containing the calculated ticks and bounds.</returns>
    /// <remarks>
    /// The actual number of ticks may vary from the target to achieve nice rounded values.
    /// Edge cases like NaN, infinity, or equal min/max are handled gracefully.
    /// </remarks>
    public static AxisTickInfo CalculateTicks(double dataMin, double dataMax, int targetTickCount = 8)
    {
      var result = new AxisTickInfo();

      // Handle edge cases
      if (double.IsNaN(dataMin) || double.IsNaN(dataMax) || 
          double.IsInfinity(dataMin) || double.IsInfinity(dataMax))
      {
        return CreateDefaultTicks(-10, 10, targetTickCount);
      }

      // Handle case where min equals max
      if (Math.Abs(dataMax - dataMin) < double.Epsilon)
      {
        double padding = Math.Abs(dataMin) * 0.1;
        if (padding < double.Epsilon) padding = 1.0;
        dataMin -= padding;
        dataMax += padding;
      }

      double range = dataMax - dataMin;
      double roughTickSpacing = range / (targetTickCount - 1);
      double magnitude = Math.Pow(10, Math.Floor(Math.Log10(roughTickSpacing)));
      double normalizedSpacing = roughTickSpacing / magnitude;
      double niceSpacing = FindNiceNumber(normalizedSpacing);
      double tickSpacing = niceSpacing * magnitude;

      double niceMin = Math.Floor(dataMin / tickSpacing) * tickSpacing;
      double niceMax = Math.Ceiling(dataMax / tickSpacing) * tickSpacing;

      result.DecimalPlaces = CalculateDecimalPlaces(tickSpacing);
      result.UseScientificNotation = ShouldUseScientificNotation(niceMin, niceMax, tickSpacing);
      result.Min = niceMin;
      result.Max = niceMax;
      result.TickSpacing = tickSpacing;
      result.Ticks = GenerateTicks(niceMin, niceMax, tickSpacing, result.DecimalPlaces, result.UseScientificNotation);

      return result;
    }

    /// <summary>
    /// Calculates ticks with additional padding around the data range.
    /// </summary>
    /// <param name="dataMin">The minimum value in the data.</param>
    /// <param name="dataMax">The maximum value in the data.</param>
    /// <param name="targetTickCount">The approximate number of ticks desired.</param>
    /// <param name="paddingPercent">The percentage of range to add as padding on each side.</param>
    /// <returns>An <see cref="AxisTickInfo"/> with padded bounds.</returns>
    public static AxisTickInfo CalculateTicksWithPadding(double dataMin, double dataMax, int targetTickCount = 8, double paddingPercent = 5)
    {
      double range = dataMax - dataMin;
      double padding = range * paddingPercent / 100.0;
      return CalculateTicks(dataMin - padding, dataMax + padding, targetTickCount);
    }

    /// <summary>
    /// Finds the smallest "nice" number that is approximately >= the given value.
    /// </summary>
    private static double FindNiceNumber(double value)
    {
      foreach (double nice in NiceNumbers)
      {
        if (nice >= value * 0.9)
          return nice;
      }
      return NiceNumbers[NiceNumbers.Length - 1];
    }

    /// <summary>
    /// Determines the number of decimal places needed for tick labels.
    /// </summary>
    private static int CalculateDecimalPlaces(double tickSpacing)
    {
      if (tickSpacing >= 1.0)
        return 0;

      double log = Math.Log10(tickSpacing);
      int decimals = (int)Math.Ceiling(-log);
      return Math.Max(0, Math.Min(decimals, 10));
    }

    /// <summary>
    /// Determines whether scientific notation should be used for tick labels.
    /// </summary>
    private static bool ShouldUseScientificNotation(double min, double max, double tickSpacing)
    {
      double maxAbs = Math.Max(Math.Abs(min), Math.Abs(max));
      return maxAbs >= 100000 || (maxAbs > 0 && maxAbs < 0.01);
    }

    /// <summary>
    /// Generates the list of tick marks between min and max.
    /// </summary>
    private static List<AxisTick> GenerateTicks(double min, double max, double spacing, int decimalPlaces, bool useScientific)
    {
      var ticks = new List<AxisTick>();
      double epsilon = spacing * 1e-10;

      for (double value = min; value <= max + epsilon; value += spacing)
      {
        double roundedValue = Math.Round(value / spacing) * spacing;
        
        if (Math.Abs(roundedValue) < spacing * 1e-10)
          roundedValue = 0;

        ticks.Add(new AxisTick
        {
          Value = roundedValue,
          Label = FormatTickLabel(roundedValue, decimalPlaces, useScientific),
          IsMajor = true
        });
      }

      return ticks;
    }

    /// <summary>
    /// Formats a tick value as a display string.
    /// </summary>
    private static string FormatTickLabel(double value, int decimalPlaces, bool useScientific)
    {
      if (Math.Abs(value) < 1e-15)
        return "0";

      if (useScientific)
        return value.ToString("E2");

      if (decimalPlaces == 0)
        return ((long)Math.Round(value)).ToString();

      return value.ToString($"F{decimalPlaces}");
    }

    /// <summary>
    /// Creates default tick information when normal calculation fails.
    /// </summary>
    private static AxisTickInfo CreateDefaultTicks(double min, double max, int tickCount)
    {
      var result = new AxisTickInfo
      {
        Min = min,
        Max = max,
        TickSpacing = (max - min) / (tickCount - 1),
        DecimalPlaces = 0,
        UseScientificNotation = false
      };

      for (int i = 0; i < tickCount; i++)
      {
        double value = min + i * result.TickSpacing;
        result.Ticks.Add(new AxisTick
        {
          Value = value,
          Label = value.ToString("F0"),
          IsMajor = true
        });
      }

      return result;
    }

    /// <summary>
    /// Calculates tick information for both X and Y axes simultaneously.
    /// </summary>
    /// <param name="xMin">Minimum X data value.</param>
    /// <param name="xMax">Maximum X data value.</param>
    /// <param name="yMin">Minimum Y data value.</param>
    /// <param name="yMax">Maximum Y data value.</param>
    /// <param name="xTickCount">Target number of X axis ticks.</param>
    /// <param name="yTickCount">Target number of Y axis ticks.</param>
    /// <returns>A tuple containing tick information for both axes.</returns>
    public static (AxisTickInfo xTicks, AxisTickInfo yTicks) CalculateAxisTicks(
      double xMin, double xMax, double yMin, double yMax, 
      int xTickCount = 10, int yTickCount = 8)
    {
      var xTicks = CalculateTicks(xMin, xMax, xTickCount);
      var yTicks = CalculateTicks(yMin, yMax, yTickCount);
      return (xTicks, yTicks);
    }
  }
}
