using System;
using System.Globalization;
using System.IO;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Factory class for loading parametric curve data from files.
  /// </summary>
  public static class CurveLoader
  {
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

    private static (LoadedParamCurve2D curve, string title) LoadParametricCurve2D(string[] lines, int index)
    {
      var curve = new LoadedParamCurve2D(index);

      curve.Title = lines[1];

      // Parse t1, t2, numPoints (metadata)
      // string[] partsT1 = lines[2].Split(' ');
      // double t1 = double.Parse(partsT1[1], CultureInfo.InvariantCulture);
      // string[] partsT2 = lines[3].Split(' ');
      // double t2 = double.Parse(partsT2[1], CultureInfo.InvariantCulture);
      // string[] partsNumPoints = lines[4].Split(' ');
      // int numPoints = int.Parse(partsNumPoints[1]);

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
