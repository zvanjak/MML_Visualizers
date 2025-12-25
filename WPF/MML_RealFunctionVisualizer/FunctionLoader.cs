using System;
using System.Globalization;
using System.IO;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Factory class for loading function data from files.
  /// </summary>
  public static class FunctionLoader
  {
    public static (ILoadedFunction? function, string? title) Load(string fileName, int index = 0)
    {
      if (!File.Exists(fileName))
        throw new FileNotFoundException($"File does not exist: {fileName}");

      string[] lines = File.ReadAllLines(fileName);
      if (lines.Length == 0)
        throw new InvalidDataException("File is empty.");

      string type = lines[0].Trim();

      return type switch
      {
        "REAL_FUNCTION" => LoadRealFunction(lines, index),
        "MULTI_REAL_FUNCTION" => LoadMultiRealFunction(lines),
        "REAL_FUNCTION_EQUALLY_SPACED" => throw new NotSupportedException("REAL_FUNCTION_EQUALLY_SPACED not yet supported"),
        "REAL_FUNCTION_VARIABLE_SPACED" => throw new NotSupportedException("REAL_FUNCTION_VARIABLE_SPACED not yet supported"),
        _ => throw new NotSupportedException($"Unsupported format: {type}")
      };
    }

    private static (ILoadedFunction function, string title) LoadRealFunction(string[] lines, int index)
    {
      var function = new SingleLoadedFunction(index);

      function.Title = lines[1];
      function.LoadedType = LoadedType.REAL_FUNCTION;

      // Parse x1, x2, numPoints (for metadata, not currently used)
      // string[] partsX1 = lines[2].Split(' ');
      // double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);
      // string[] partsX2 = lines[3].Split(' ');
      // double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);
      // string[] partsNumPoints = lines[4].Split(' ');
      // int numPoints = int.Parse(partsNumPoints[1]);

      for (int i = 5; i < lines.Length; i++)
      {
        if (string.IsNullOrWhiteSpace(lines[i])) continue;

        string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length < 2) continue;

        double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
        double y = double.Parse(parts[1], CultureInfo.InvariantCulture);

        function.AddPoint(x, y);
      }

      return (function, function.Title);
    }

    private static (ILoadedFunction function, string title) LoadMultiRealFunction(string[] lines)
    {
      var function = new MultiLoadedFunction();

      function.Title = lines[1];
      int dim = int.Parse(lines[2]);

      // Parse legend
      string[] legend = new string[dim];
      for (int i = 0; i < dim; i++)
      {
        legend[i] = lines[3 + i];
      }
      function.Legend = legend;

      // Parse x range and num points
      string[] partsX1 = lines[3 + dim].Split(' ', StringSplitOptions.RemoveEmptyEntries);
      // double xMin = double.Parse(partsX1[1], CultureInfo.InvariantCulture);

      string[] partsX2 = lines[4 + dim].Split(' ', StringSplitOptions.RemoveEmptyEntries);
      // double xMax = double.Parse(partsX2[1], CultureInfo.InvariantCulture);

      string[] partsNumPoints = lines[5 + dim].Split(' ', StringSplitOptions.RemoveEmptyEntries);
      int numPoints = int.Parse(partsNumPoints[1], CultureInfo.InvariantCulture);

      // Create data structures
      var xValues = new MML.Vector(numPoints);
      var yValues = new MML.Matrix(dim, numPoints);

      int cnt = 0;
      for (int i = 6 + dim; i < lines.Length && cnt < numPoints; i++)
      {
        if (string.IsNullOrWhiteSpace(lines[i])) continue;

        string[] parts = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length < dim + 1) continue;

        double x = double.Parse(parts[0], CultureInfo.InvariantCulture);
        xValues.Elements[cnt] = x;

        for (int j = 0; j < dim; j++)
        {
          double y = double.Parse(parts[j + 1], CultureInfo.InvariantCulture);
          yValues.SetElemAt(j, cnt, y);
        }
        cnt++;
      }

      function.SetData(xValues, yValues);

      return (function, function.Title);
    }
  }
}