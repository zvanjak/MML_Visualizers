using System;
using System.Globalization;
using System.IO;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Factory class responsible for loading function data from text files.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class implements the Factory pattern to create appropriate function objects
  /// based on the file format. It supports multiple file formats:
  /// </para>
  /// <list type="table">
  ///   <listheader>
  ///     <term>Format Type</term>
  ///     <description>Description</description>
  ///   </listheader>
  ///   <item>
  ///     <term>REAL_FUNCTION</term>
  ///     <description>Single function with (x, y) data points</description>
  ///   </item>
  ///   <item>
  ///     <term>MULTI_REAL_FUNCTION</term>
  ///     <description>Multiple functions sharing the same X values</description>
  ///   </item>
  /// </list>
  /// </remarks>
  /// <example>
  /// <code>
  /// var (function, title) = FunctionLoader.Load("data/sine.txt", 0);
  /// if (function != null)
  /// {
  ///     function.Draw(canvas, coordParams);
  /// }
  /// </code>
  /// </example>
  public static class FunctionLoader
  {
    /// <summary>
    /// Loads function data from a file and creates the appropriate function object.
    /// </summary>
    /// <param name="fileName">The path to the data file.</param>
    /// <param name="index">The index used for assigning default colors (default: 0).</param>
    /// <returns>
    /// A tuple containing the loaded function and its title, or (null, null) if loading fails.
    /// </returns>
    /// <exception cref="FileNotFoundException">Thrown when the specified file does not exist.</exception>
    /// <exception cref="InvalidDataException">Thrown when the file is empty.</exception>
    /// <exception cref="NotSupportedException">Thrown when the file format is not supported.</exception>
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

    /// <summary>
    /// Loads a single real function from the file data.
    /// </summary>
    /// <param name="lines">The lines read from the file.</param>
    /// <param name="index">The index for default color assignment.</param>
    /// <returns>A tuple with the loaded function and its title.</returns>
    /// <remarks>
    /// Expected file format:
    /// <code>
    /// REAL_FUNCTION
    /// Title
    /// x1: [min_x]
    /// x2: [max_x]
    /// NumPoints: [count]
    /// x0 y0
    /// x1 y1
    /// ...
    /// </code>
    /// </remarks>
    private static (ILoadedFunction function, string title) LoadRealFunction(string[] lines, int index)
    {
      var function = new SingleLoadedFunction(index);

      function.Title = lines[1];
      function.LoadedType = LoadedType.REAL_FUNCTION;

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

    /// <summary>
    /// Loads a multi-function data set from the file data.
    /// </summary>
    /// <param name="lines">The lines read from the file.</param>
    /// <returns>A tuple with the loaded multi-function and its title.</returns>
    /// <remarks>
    /// Expected file format:
    /// <code>
    /// MULTI_REAL_FUNCTION
    /// Title
    /// [dimension]
    /// Legend1
    /// Legend2
    /// ...
    /// x1: [min_x]
    /// x2: [max_x]
    /// NumPoints: [count]
    /// x0 y0_1 y0_2 ...
    /// x1 y1_1 y1_2 ...
    /// ...
    /// </code>
    /// </remarks>
    private static (ILoadedFunction function, string title) LoadMultiRealFunction(string[] lines)
    {
      var function = new MultiLoadedFunction();

      function.Title = lines[1];
      int dim = int.Parse(lines[2]);

      // Parse legend labels for each function
      string[] legend = new string[dim];
      for (int i = 0; i < dim; i++)
      {
        legend[i] = lines[3 + i];
      }
      function.Legend = legend;

      // Parse metadata
      string[] partsNumPoints = lines[5 + dim].Split(' ', StringSplitOptions.RemoveEmptyEntries);
      int numPoints = int.Parse(partsNumPoints[1], CultureInfo.InvariantCulture);

      // Create data structures
      var xValues = new MML.Vector(numPoints);
      var yValues = new MML.Matrix(dim, numPoints);

      // Parse data points
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