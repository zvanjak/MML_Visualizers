using System;
using System.Globalization;
using System.IO;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Factory class responsible for loading parametric curve data from text files.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class implements the Factory pattern to create parametric curve objects
  /// from data files. It supports the following file format:
  /// </para>
  /// <list type="table">
  ///   <listheader>
  ///     <term>Format Type</term>
  ///     <description>Description</description>
  ///   </listheader>
  ///   <item>
  ///     <term>PARAMETRIC_CURVE_CARTESIAN_2D</term>
  ///     <description>2D parametric curve with (t, x, y) data points</description>
  ///   </item>
  /// </list>
  /// </remarks>
  /// <example>
  /// <code>
  /// var (curve, title) = CurveLoader.Load("data/spiral.txt", 0);
  /// if (curve != null)
  /// {
  ///     curve.Draw(canvas, coordParams);
  /// }
  /// </code>
  /// </example>
  public static class CurveLoader
  {
    /// <summary>
    /// Loads parametric curve data from a file and creates a curve object.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <param name="index">The index used for assigning default colors (default: 0).</param>
    /// <returns>
    /// A tuple containing the loaded curve and its title, or (null, null) if loading fails.
    /// </returns>
    /// <exception cref="FileNotFoundException">Thrown when the specified file does not exist.</exception>
    /// <exception cref="InvalidDataException">Thrown when the file is empty.</exception>
    /// <exception cref="NotSupportedException">Thrown when the file format is not supported.</exception>
    public static (LoadedParamCurve2D? curve, string? title) Load(string fileName, int index = 0)
    {
      if (!File.Exists(fileName))
        throw new FileNotFoundException($"File does not exist: {fileName}");

      string[] lines = File.ReadAllLines(fileName);
      if (lines.Length == 0)
        throw new InvalidDataException("File is empty.");

      string type = lines[0].Trim();

      return type switch
      {
        "PARAMETRIC_CURVE_CARTESIAN_2D" => LoadParametricCurve2D(lines, index),
        _ => throw new NotSupportedException($"Unsupported format: {type}")
      };
    }

    /// <summary>
    /// Loads a 2D parametric curve from the file data.
    /// </summary>
    /// <param name="lines">The lines read from the file.</param>
    /// <param name="index">The index for default color assignment.</param>
    /// <returns>A tuple with the loaded curve and its title.</returns>
    /// <remarks>
    /// Expected file format:
    /// <code>
    /// PARAMETRIC_CURVE_CARTESIAN_2D
    /// Title
    /// t1: [start_parameter]
    /// t2: [end_parameter]
    /// NumPoints: [count]
    /// t0 x0 y0
    /// t1 x1 y1
    /// t2 x2 y2
    /// ...
    /// </code>
    /// <para>
    /// Each data line contains three space-separated values:
    /// </para>
    /// <list type="bullet">
    ///   <item><description>t - the parameter value</description></item>
    ///   <item><description>x - the X coordinate at this parameter value</description></item>
    ///   <item><description>y - the Y coordinate at this parameter value</description></item>
    /// </list>
    /// <para>
    /// All numeric values should use invariant culture (dot as decimal separator).
    /// </para>
    /// </remarks>
    private static (LoadedParamCurve2D curve, string title) LoadParametricCurve2D(string[] lines, int index)
    {
      var curve = new LoadedParamCurve2D(index);

      curve.Title = lines[1];

      for (int i = 5; i < lines.Length; i++)
      {
        if (string.IsNullOrWhiteSpace(lines[i])) continue;

        string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length < 3) continue;

        double t = double.Parse(parts[0], CultureInfo.InvariantCulture);
        double x = double.Parse(parts[1], CultureInfo.InvariantCulture);
        double y = double.Parse(parts[2], CultureInfo.InvariantCulture);

        curve.AddPoint(t, x, y);
      }

      return (curve, curve.Title);
    }
  }
}
