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
  /// Represents multiple related real functions sharing the same X values, loaded from a data file.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class stores a set of functions where each function shares the same X coordinates
  /// but has different Y values. This is useful for visualizing related data series such as:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Position, velocity, and acceleration over time</description></item>
  ///   <item><description>Multiple solutions to the same problem</description></item>
  ///   <item><description>Comparison of different algorithms</description></item>
  /// </list>
  /// <para>
  /// Each individual function within the set can have its own visibility state and draw style.
  /// </para>
  /// </remarks>
  public class MultiLoadedFunction : ILoadedFunction
  {
    private string[] _legend = Array.Empty<string>();
    private MML.Vector? _xValues;
    private MML.Matrix? _yValues;
    private List<FunctionDrawStyle> _drawStyles = new List<FunctionDrawStyle>();
    private List<bool> _visibilityFlags = new List<bool>();

    /// <summary>
    /// Gets or sets the overall title of the multi-function data set.
    /// </summary>
    public string Title { get; set; } = "";
    
    /// <summary>
    /// Gets or sets the legend labels for each individual function.
    /// </summary>
    public string[] Legend
    {
      get => _legend;
      set => _legend = value ?? Array.Empty<string>();
    }

    /// <summary>
    /// Gets the number of individual functions in this data set.
    /// </summary>
    public int Dimension => _yValues?.Rows ?? 0;

    /// <summary>
    /// Gets the drawing styles for each function as a read-only collection.
    /// </summary>
    public IReadOnlyList<FunctionDrawStyle> DrawStyles => _drawStyles;

    /// <summary>
    /// Sets the X and Y data for the multi-function set.
    /// </summary>
    /// <param name="xValues">Vector containing the shared X coordinate values.</param>
    /// <param name="yValues">Matrix where each row represents a different function's Y values.</param>
    /// <exception cref="ArgumentNullException">Thrown when xValues or yValues is null.</exception>
    /// <remarks>
    /// This method initializes default draw styles (with DashDot line style) and visibility flags
    /// for each function in the data set.
    /// </remarks>
    public void SetData(MML.Vector xValues, MML.Matrix yValues)
    {
      _xValues = xValues ?? throw new ArgumentNullException(nameof(xValues));
      _yValues = yValues ?? throw new ArgumentNullException(nameof(yValues));

      // Initialize default draw styles and visibility for each function
      _drawStyles.Clear();
      _visibilityFlags.Clear();
      for (int i = 0; i < yValues.Rows; i++)
      {
        var style = FunctionDrawStyle.CreateDefault(i);
        style.LineStyle = LineStyle.DashDot; // Default for multi-function
        _drawStyles.Add(style);
        _visibilityFlags.Add(true);
      }
    }

    /// <summary>
    /// Sets a custom drawing style for a specific function.
    /// </summary>
    /// <param name="functionIndex">The zero-based index of the function.</param>
    /// <param name="style">The drawing style to apply.</param>
    public void SetDrawStyle(int functionIndex, FunctionDrawStyle style)
    {
      if (functionIndex >= 0 && functionIndex < _drawStyles.Count)
        _drawStyles[functionIndex] = style;
    }

    /// <summary>
    /// Sets the visibility state of a specific function.
    /// </summary>
    /// <param name="functionIndex">The zero-based index of the function.</param>
    /// <param name="isVisible">True to show the function, false to hide it.</param>
    public void SetFunctionVisibility(int functionIndex, bool isVisible)
    {
      if (functionIndex >= 0 && functionIndex < _visibilityFlags.Count)
        _visibilityFlags[functionIndex] = isVisible;
    }

    /// <summary>
    /// Gets the visibility state of a specific function.
    /// </summary>
    /// <param name="functionIndex">The zero-based index of the function.</param>
    /// <returns>True if the function is visible, false otherwise.</returns>
    public bool GetFunctionVisibility(int functionIndex)
    {
      if (functionIndex >= 0 && functionIndex < _visibilityFlags.Count)
        return _visibilityFlags[functionIndex];
      return true;
    }

    /// <summary>
    /// Gets the shared X coordinate values.
    /// </summary>
    /// <returns>The vector of X values, or null if data hasn't been set.</returns>
    public MML.Vector? GetXValues() => _xValues;

    /// <summary>
    /// Gets the Y values matrix where each row is a different function.
    /// </summary>
    /// <returns>The matrix of Y values, or null if data hasn't been set.</returns>
    public MML.Matrix? GetYValues() => _yValues;

    /// <inheritdoc/>
    /// <exception cref="ArgumentOutOfRangeException">Thrown when index is outside valid range.</exception>
    public string GetFunctionTitle(int index)
    {
      if (index < 0 || index >= Dimension)
        throw new ArgumentOutOfRangeException(nameof(index));
      
      return index < _legend.Length ? _legend[index] : $"Function {index + 1}";
    }

    /// <inheritdoc/>
    public int GetNumPoints() => _xValues?.Elements?.Length ?? 0;

    /// <inheritdoc/>
    public double GetMinX()
    {
      if (_xValues?.Elements == null || _xValues.Elements.Length == 0)
        return 0;
      return _xValues.Elements.Min();
    }

    /// <inheritdoc/>
    public double GetMaxX()
    {
      if (_xValues?.Elements == null || _xValues.Elements.Length == 0)
        return 0;
      return _xValues.Elements.Max();
    }

    /// <inheritdoc/>
    /// <remarks>
    /// Searches across all functions to find the global minimum Y value.
    /// </remarks>
    public double GetMinY()
    {
      if (_yValues == null || _yValues.Rows == 0 || _yValues.Cols == 0)
        return 0;

      double min = _yValues.ElemAt(0, 0);
      for (int row = 0; row < _yValues.Rows; row++)
        for (int col = 0; col < _yValues.Cols; col++)
          if (_yValues.ElemAt(row, col) < min)
            min = _yValues.ElemAt(row, col);

      return min;
    }

    /// <inheritdoc/>
    /// <remarks>
    /// Searches across all functions to find the global maximum Y value.
    /// </remarks>
    public double GetMaxY()
    {
      if (_yValues == null || _yValues.Rows == 0 || _yValues.Cols == 0)
        return 0;

      double max = _yValues.ElemAt(0, 0);
      for (int row = 0; row < _yValues.Rows; row++)
        for (int col = 0; col < _yValues.Cols; col++)
          if (_yValues.ElemAt(row, col) > max)
            max = _yValues.ElemAt(row, col);

      return max;
    }

    /// <summary>
    /// Renders all visible functions on the specified canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    /// <remarks>
    /// Each function is rendered as a separate polyline with its own style.
    /// Functions with visibility set to false are skipped.
    /// </remarks>
    public void Draw(Canvas canvas, CoordSystemParams coordParams)
    {
      if (_xValues?.Elements == null || _yValues == null)
        return;

      int numPoints = _xValues.Elements.Length;
      if (numPoints < 2) return;

      // Draw each function as a separate Polyline (if visible)
      for (int funcIndex = 0; funcIndex < _yValues.Rows; funcIndex++)
      {
        // Skip if this function is not visible
        if (funcIndex < _visibilityFlags.Count && !_visibilityFlags[funcIndex])
          continue;

        var style = funcIndex < _drawStyles.Count 
          ? _drawStyles[funcIndex] 
          : FunctionDrawStyle.CreateDefault(funcIndex);

        Polyline polyline = new Polyline
        {
          Stroke = style.Stroke,
          StrokeThickness = style.StrokeThickness,
          StrokeDashArray = style.GetDashArray()
        };

        for (int i = 0; i < numPoints; i++)
        {
          Point screenPoint = CoordTransform.WorldToScreen(
            _xValues.Elements[i], 
            _yValues.ElemAt(funcIndex, i), 
            coordParams);
          polyline.Points.Add(screenPoint);
        }

        canvas.Children.Add(polyline);

        // Optionally draw points
        if (style.ShowPoints)
        {
          DrawPoints(canvas, coordParams, funcIndex, style);
        }
      }
    }

    /// <summary>
    /// Draws individual data points as circles for a specific function.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    /// <param name="funcIndex">The index of the function to draw points for.</param>
    /// <param name="style">The drawing style to use.</param>
    private void DrawPoints(Canvas canvas, CoordSystemParams coordParams, int funcIndex, FunctionDrawStyle style)
    {
      if (_xValues?.Elements == null || _yValues == null) return;

      for (int i = 0; i < _xValues.Elements.Length; i++)
      {
        Point screenPoint = CoordTransform.WorldToScreen(
          _xValues.Elements[i],
          _yValues.ElemAt(funcIndex, i),
          coordParams);

        Ellipse point = new Ellipse
        {
          Width = style.PointRadius * 2,
          Height = style.PointRadius * 2,
          Fill = style.Stroke
        };

        Canvas.SetLeft(point, screenPoint.X - style.PointRadius);
        Canvas.SetTop(point, screenPoint.Y - style.PointRadius);
        canvas.Children.Add(point);
      }
    }
  }
}
