using System;
using System.Collections.Generic;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Represents a single tick on an axis.
  /// </summary>
  public class AxisTick
  {
    public double Value { get; set; }
    public string Label { get; set; } = "";
    public bool IsMajor { get; set; } = true;
  }

  /// <summary>
  /// Contains the calculated tick information for an axis.
  /// </summary>
  public class AxisTickInfo
  {
    public double Min { get; set; }
    public double Max { get; set; }
    public double TickSpacing { get; set; }
    public List<AxisTick> Ticks { get; set; } = new List<AxisTick>();
    public int DecimalPlaces { get; set; }
    public bool UseScientificNotation { get; set; }
  }

  /// <summary>
  /// Calculates optimal axis ticks with rounded values.
  /// </summary>
  public static class AxisTickCalculator
  {
    // Preferred tick intervals (normalized to 1-10 range)
    private static readonly double[] NiceNumbers = { 1.0, 2.0, 2.5, 5.0, 10.0 };

    /// <summary>
    /// Calculates nice tick values for an axis.
    /// </summary>
    /// <param name="dataMin">Minimum data value</param>
    /// <param name="dataMax">Maximum data value</param>
    /// <param name="targetTickCount">Desired number of ticks (approximate)</param>
    /// <returns>Axis tick information with rounded values</returns>
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

      // Calculate the rough tick spacing
      double roughTickSpacing = range / (targetTickCount - 1);

      // Find the magnitude (power of 10)
      double magnitude = Math.Pow(10, Math.Floor(Math.Log10(roughTickSpacing)));

      // Normalize the rough tick spacing to 1-10 range
      double normalizedSpacing = roughTickSpacing / magnitude;

      // Find the nearest "nice" number
      double niceSpacing = FindNiceNumber(normalizedSpacing);

      // Calculate the actual tick spacing
      double tickSpacing = niceSpacing * magnitude;

      // Calculate the nice min and max (rounded to tick spacing)
      double niceMin = Math.Floor(dataMin / tickSpacing) * tickSpacing;
      double niceMax = Math.Ceiling(dataMax / tickSpacing) * tickSpacing;

      // Determine formatting
      result.DecimalPlaces = CalculateDecimalPlaces(tickSpacing);
      result.UseScientificNotation = ShouldUseScientificNotation(niceMin, niceMax, tickSpacing);

      result.Min = niceMin;
      result.Max = niceMax;
      result.TickSpacing = tickSpacing;

      // Generate the ticks
      result.Ticks = GenerateTicks(niceMin, niceMax, tickSpacing, result.DecimalPlaces, result.UseScientificNotation);

      return result;
    }

    /// <summary>
    /// Calculates ticks with a specific range that includes padding.
    /// </summary>
    public static AxisTickInfo CalculateTicksWithPadding(double dataMin, double dataMax, int targetTickCount = 8, double paddingPercent = 5)
    {
      double range = dataMax - dataMin;
      double padding = range * paddingPercent / 100.0;
      return CalculateTicks(dataMin - padding, dataMax + padding, targetTickCount);
    }

    private static double FindNiceNumber(double value)
    {
      // Find the smallest nice number that is >= value
      foreach (double nice in NiceNumbers)
      {
        if (nice >= value * 0.9) // Allow 10% tolerance
          return nice;
      }
      return NiceNumbers[NiceNumbers.Length - 1];
    }

    private static int CalculateDecimalPlaces(double tickSpacing)
    {
      if (tickSpacing >= 1.0)
        return 0;

      // Calculate number of decimal places needed
      double log = Math.Log10(tickSpacing);
      int decimals = (int)Math.Ceiling(-log);
      return Math.Max(0, Math.Min(decimals, 10)); // Clamp to reasonable range
    }

    private static bool ShouldUseScientificNotation(double min, double max, double tickSpacing)
    {
      double maxAbs = Math.Max(Math.Abs(min), Math.Abs(max));
      
      // Use scientific notation for very large or very small numbers
      return maxAbs >= 100000 || (maxAbs > 0 && maxAbs < 0.01);
    }

    private static List<AxisTick> GenerateTicks(double min, double max, double spacing, int decimalPlaces, bool useScientific)
    {
      var ticks = new List<AxisTick>();

      // Add a small epsilon to handle floating point errors
      double epsilon = spacing * 1e-10;

      for (double value = min; value <= max + epsilon; value += spacing)
      {
        // Round to avoid floating point errors
        double roundedValue = Math.Round(value / spacing) * spacing;
        
        // Handle very small values that should be zero
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

    private static string FormatTickLabel(double value, int decimalPlaces, bool useScientific)
    {
      if (Math.Abs(value) < 1e-15)
        return "0";

      if (useScientific)
      {
        return value.ToString("E2");
      }

      if (decimalPlaces == 0)
      {
        return ((long)Math.Round(value)).ToString();
      }

      return value.ToString($"F{decimalPlaces}");
    }

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
    /// Calculates both X and Y axis ticks for a coordinate system.
    /// </summary>
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
