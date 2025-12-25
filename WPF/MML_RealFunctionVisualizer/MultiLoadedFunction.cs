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
  public class MultiLoadedFunction : ILoadedFunction
  {
    private string[] _legend = Array.Empty<string>();
    private MML.Vector? _xValues;
    private MML.Matrix? _yValues;
    private List<FunctionDrawStyle> _drawStyles = new List<FunctionDrawStyle>();

    public string Title { get; set; } = "";
    
    public string[] Legend
    {
      get => _legend;
      set => _legend = value ?? Array.Empty<string>();
    }

    public int Dimension => _yValues?.Rows ?? 0;

    public IReadOnlyList<FunctionDrawStyle> DrawStyles => _drawStyles;

    public void SetData(MML.Vector xValues, MML.Matrix yValues)
    {
      _xValues = xValues ?? throw new ArgumentNullException(nameof(xValues));
      _yValues = yValues ?? throw new ArgumentNullException(nameof(yValues));

      // Initialize default draw styles for each function
      _drawStyles.Clear();
      for (int i = 0; i < yValues.Rows; i++)
      {
        var style = FunctionDrawStyle.CreateDefault(i);
        style.LineStyle = LineStyle.DashDot; // Default for multi-function
        _drawStyles.Add(style);
      }
    }

    public void SetDrawStyle(int functionIndex, FunctionDrawStyle style)
    {
      if (functionIndex >= 0 && functionIndex < _drawStyles.Count)
        _drawStyles[functionIndex] = style;
    }

    public MML.Vector? GetXValues() => _xValues;
    public MML.Matrix? GetYValues() => _yValues;

    public string GetFunctionTitle(int index)
    {
      if (index < 0 || index >= Dimension)
        throw new ArgumentOutOfRangeException(nameof(index));
      
      return index < _legend.Length ? _legend[index] : $"Function {index + 1}";
    }

    public int GetNumPoints() => _xValues?.Elements?.Length ?? 0;

    public double GetMinX()
    {
      if (_xValues?.Elements == null || _xValues.Elements.Length == 0)
        return 0;
      return _xValues.Elements.Min();
    }

    public double GetMaxX()
    {
      if (_xValues?.Elements == null || _xValues.Elements.Length == 0)
        return 0;
      return _xValues.Elements.Max();
    }

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

    public void Draw(Canvas canvas, CoordSystemParams coordParams)
    {
      if (_xValues?.Elements == null || _yValues == null)
        return;

      int numPoints = _xValues.Elements.Length;
      if (numPoints < 2) return;

      // Draw each function as a separate Polyline
      for (int funcIndex = 0; funcIndex < _yValues.Rows; funcIndex++)
      {
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
