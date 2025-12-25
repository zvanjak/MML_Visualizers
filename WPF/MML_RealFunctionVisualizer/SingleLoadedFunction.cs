using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Represents a single real function y = f(x) loaded from a data file.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class stores discrete (x, y) data points representing a single-valued function.
  /// It supports customizable drawing styles and visibility toggling for interactive visualization.
  /// </para>
  /// <para>
  /// The function is rendered using a <see cref="Polyline"/> for optimal performance when
  /// dealing with large numbers of data points.
  /// </para>
  /// </remarks>
  /// <example>
  /// <code>
  /// var function = new SingleLoadedFunction(0);
  /// function.Title = "Sin(x)";
  /// function.AddPoint(0, 0);
  /// function.AddPoint(Math.PI/2, 1);
  /// function.AddPoint(Math.PI, 0);
  /// function.Draw(canvas, coordParams);
  /// </code>
  /// </example>
  public class SingleLoadedFunction : ILoadedFunction
  {
    private readonly List<double> _xVals = new List<double>();
    private readonly List<double> _yVals = new List<double>();
    private readonly int _index;

    /// <summary>
    /// Gets or sets the display title of the function.
    /// </summary>
    public string Title { get; set; } = "";

    /// <summary>
    /// Gets or sets the type of function data loaded (e.g., equally spaced, variable spaced).
    /// </summary>
    public LoadedType LoadedType { get; set; }

    /// <summary>
    /// Gets the X coordinate values as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> XValues => _xVals;

    /// <summary>
    /// Gets the Y coordinate values as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> YValues => _yVals;

    /// <summary>
    /// Gets the dimension of this function data set. Always returns 1 for single functions.
    /// </summary>
    public int Dimension => 1;

    /// <summary>
    /// Gets or sets the drawing style (color, thickness, line style) for this function.
    /// </summary>
    public FunctionDrawStyle DrawStyle { get; set; }

    /// <summary>
    /// Gets or sets whether this function should be rendered. 
    /// When false, the function is hidden from the visualization.
    /// </summary>
    public bool IsVisible { get; set; } = true;

    /// <summary>
    /// Initializes a new instance of the <see cref="SingleLoadedFunction"/> class.
    /// </summary>
    /// <param name="index">The index used to assign a default color from the palette.</param>
    public SingleLoadedFunction(int index)
    {
      _index = index;
      DrawStyle = FunctionDrawStyle.CreateDefault(index);
    }

    /// <summary>
    /// Adds a data point to the function.
    /// </summary>
    /// <param name="x">The X coordinate value.</param>
    /// <param name="y">The Y coordinate value (function output).</param>
    public void AddPoint(double x, double y)
    {
      _xVals.Add(x);
      _yVals.Add(y);
    }

    /// <inheritdoc/>
    public string GetFunctionTitle(int index) => Title;

    /// <inheritdoc/>
    public int GetNumPoints() => _xVals.Count;

    /// <inheritdoc/>
    public double GetMinX() => _xVals.Count > 0 ? _xVals.Min() : 0;

    /// <inheritdoc/>
    public double GetMaxX() => _xVals.Count > 0 ? _xVals.Max() : 0;

    /// <inheritdoc/>
    public double GetMinY() => _yVals.Count > 0 ? _yVals.Min() : 0;

    /// <inheritdoc/>
    public double GetMaxY() => _yVals.Count > 0 ? _yVals.Max() : 0;

    /// <summary>
    /// Renders the function on the specified canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    /// <remarks>
    /// The function is rendered as a connected polyline. If <see cref="IsVisible"/> is false
    /// or there are fewer than 2 data points, nothing is drawn.
    /// </remarks>
    public void Draw(Canvas canvas, CoordSystemParams coordParams)
    {
      // Skip drawing if not visible
      if (!IsVisible) return;
      if (_xVals.Count < 2) return;

      // Use Polyline for better performance
      Polyline polyline = new Polyline
      {
        Stroke = DrawStyle.Stroke,
        StrokeThickness = DrawStyle.StrokeThickness,
        StrokeDashArray = DrawStyle.GetDashArray()
      };

      for (int i = 0; i < _xVals.Count; i++)
      {
        Point screenPoint = CoordTransform.WorldToScreen(_xVals[i], _yVals[i], coordParams);
        polyline.Points.Add(screenPoint);
      }

      canvas.Children.Add(polyline);

      // Optionally draw points
      if (DrawStyle.ShowPoints)
      {
        DrawPoints(canvas, coordParams);
      }
    }

    /// <summary>
    /// Draws individual data points as circles on the canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    private void DrawPoints(Canvas canvas, CoordSystemParams coordParams)
    {
      for (int i = 0; i < _xVals.Count; i++)
      {
        Point screenPoint = CoordTransform.WorldToScreen(_xVals[i], _yVals[i], coordParams);
        
        Ellipse point = new Ellipse
        {
          Width = DrawStyle.PointRadius * 2,
          Height = DrawStyle.PointRadius * 2,
          Fill = DrawStyle.Stroke
        };

        Canvas.SetLeft(point, screenPoint.X - DrawStyle.PointRadius);
        Canvas.SetTop(point, screenPoint.Y - DrawStyle.PointRadius);
        canvas.Children.Add(point);
      }
    }
  }
}
