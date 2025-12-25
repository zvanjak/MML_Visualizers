using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Represents a 2D parametric curve loaded from a data file.
  /// </summary>
  /// <remarks>
  /// <para>
  /// A parametric curve in 2D is defined by two functions:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>x = x(t) - the X coordinate as a function of parameter t</description></item>
  ///   <item><description>y = y(t) - the Y coordinate as a function of parameter t</description></item>
  /// </list>
  /// <para>
  /// This class stores discrete samples of the curve at various parameter values and
  /// provides methods for rendering the curve on a WPF canvas. The curve is rendered
  /// using a <see cref="Polyline"/> for optimal performance with large numbers of points.
  /// </para>
  /// <para>
  /// Examples of parametric curves include:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Circles: x(t) = cos(t), y(t) = sin(t)</description></item>
  ///   <item><description>Spirals: x(t) = t·cos(t), y(t) = t·sin(t)</description></item>
  ///   <item><description>Lissajous figures: x(t) = sin(at), y(t) = sin(bt)</description></item>
  ///   <item><description>Cycloids: x(t) = r(t - sin(t)), y(t) = r(1 - cos(t))</description></item>
  /// </list>
  /// </remarks>
  /// <example>
  /// <code>
  /// var curve = new LoadedParamCurve2D(0);
  /// curve.Title = "Unit Circle";
  /// for (int i = 0; i &lt;= 100; i++)
  /// {
  ///     double t = 2 * Math.PI * i / 100;
  ///     curve.AddPoint(t, Math.Cos(t), Math.Sin(t));
  /// }
  /// curve.Draw(canvas, coordParams);
  /// </code>
  /// </example>
  public class LoadedParamCurve2D : ILoadedCurve2D
  {
    private readonly List<double> _tVals = new List<double>();
    private readonly List<double> _xVals = new List<double>();
    private readonly List<double> _yVals = new List<double>();
    private readonly int _index;

    /// <summary>
    /// Gets or sets the display title of the parametric curve.
    /// </summary>
    public string Title { get; set; } = "";

    /// <summary>
    /// Gets or sets the drawing style (color, thickness, line style) for this curve.
    /// </summary>
    public CurveDrawStyle DrawStyle { get; set; }

    /// <summary>
    /// Gets or sets whether this curve should be rendered.
    /// When false, the curve is hidden from the visualization.
    /// </summary>
    public bool IsVisible { get; set; } = true;

    /// <summary>
    /// Gets the parameter values (t) as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> TValues => _tVals;

    /// <summary>
    /// Gets the X coordinate values as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> XValues => _xVals;

    /// <summary>
    /// Gets the Y coordinate values as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> YValues => _yVals;

    /// <summary>
    /// Initializes a new instance of the <see cref="LoadedParamCurve2D"/> class.
    /// </summary>
    /// <param name="index">The index used to assign a default color from the palette.</param>
    public LoadedParamCurve2D(int index = 0)
    {
      _index = index;
      DrawStyle = CurveDrawStyle.CreateDefault(index);
    }

    /// <summary>
    /// Adds a sampled point to the parametric curve.
    /// </summary>
    /// <param name="t">The parameter value at this sample.</param>
    /// <param name="x">The X coordinate value x(t).</param>
    /// <param name="y">The Y coordinate value y(t).</param>
    public void AddPoint(double t, double x, double y)
    {
      _tVals.Add(t);
      _xVals.Add(x);
      _yVals.Add(y);
    }

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

    /// <inheritdoc/>
    public double GetMinT() => _tVals.Count > 0 ? _tVals.Min() : 0;

    /// <inheritdoc/>
    public double GetMaxT() => _tVals.Count > 0 ? _tVals.Max() : 0;

    /// <summary>
    /// Renders the parametric curve on the specified canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    /// <remarks>
    /// The curve is rendered as a connected polyline through all sampled points.
    /// If <see cref="IsVisible"/> is false or there are fewer than 2 points, nothing is drawn.
    /// </remarks>
    public void Draw(Canvas canvas, CoordSystemParams coordParams)
    {
      if (!IsVisible) return;
      if (_xVals.Count < 2) return;

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

      if (DrawStyle.ShowPoints)
      {
        DrawPoints(canvas, coordParams);
      }
    }

    /// <summary>
    /// Draws individual sampled points as circles on the canvas.
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
