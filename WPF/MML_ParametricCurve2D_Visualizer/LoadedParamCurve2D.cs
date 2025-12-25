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
  /// Represents a loaded 2D parametric curve with proper encapsulation.
  /// </summary>
  public class LoadedParamCurve2D : ILoadedCurve2D
  {
    private readonly List<double> _tVals = new List<double>();
    private readonly List<double> _xVals = new List<double>();
    private readonly List<double> _yVals = new List<double>();
    private readonly int _index;

    public string Title { get; set; } = "";
    public CurveDrawStyle DrawStyle { get; set; }

    public IReadOnlyList<double> TValues => _tVals;
    public IReadOnlyList<double> XValues => _xVals;
    public IReadOnlyList<double> YValues => _yVals;

    public LoadedParamCurve2D(int index = 0)
    {
      _index = index;
      DrawStyle = CurveDrawStyle.CreateDefault(index);
    }

    public void AddPoint(double t, double x, double y)
    {
      _tVals.Add(t);
      _xVals.Add(x);
      _yVals.Add(y);
    }

    public int GetNumPoints() => _xVals.Count;

    public double GetMinX() => _xVals.Count > 0 ? _xVals.Min() : 0;
    public double GetMaxX() => _xVals.Count > 0 ? _xVals.Max() : 0;
    public double GetMinY() => _yVals.Count > 0 ? _yVals.Min() : 0;
    public double GetMaxY() => _yVals.Count > 0 ? _yVals.Max() : 0;
    public double GetMinT() => _tVals.Count > 0 ? _tVals.Min() : 0;
    public double GetMaxT() => _tVals.Count > 0 ? _tVals.Max() : 0;

    public void Draw(Canvas canvas, CoordSystemParams coordParams)
    {
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
