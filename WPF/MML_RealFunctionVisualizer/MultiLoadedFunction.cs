﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  class MultiLoadedFunction : LoadedFunction
  {
    public string _title;
    public string[] _legend;

    public MML.Vector _multiFuncX;
    public MML.Matrix _multiFuncY;

    public int GetDimension()
    {
      return _multiFuncY.Rows;
    }

    public string GetFunctionTitle(int i)
    {
      if (i < 0 || i >= _multiFuncY.Rows)
        throw new ArgumentOutOfRangeException("Index out of range for function title retrieval.");
      return _legend[i];
    }

    public override int GetNumPoints()
    {
      return _multiFuncX.Elements.Length;
    }

    public override double GetMinX()
    {
      return _multiFuncX.Elements.Min();
    }

    public override double GetMaxX()
    {
      return _multiFuncX.Elements.Max();
    }

    public override double GetMinY()
    {
      double min = _multiFuncY.ElemAt(0, 0);
      for (int j = 0; j < _multiFuncY.Rows; j++)
        for (int i = 0; i < _multiFuncY.Cols; i++)
          if (_multiFuncY.ElemAt(j, i) < min)
            min = _multiFuncY.ElemAt(j, i);

      return min;
    }
    public override double GetMaxY()
    {
      double max = _multiFuncY.ElemAt(0, 0);
      for (int j = 0; j < _multiFuncY.Rows; j++)
      for (int i = 0; i < _multiFuncY.Cols; i++)
        if (_multiFuncY.ElemAt(j, i) > max)
          max = _multiFuncY.ElemAt(j, i);

      return max;
    }
    public override void Draw(Canvas mainCanvas, CoordSystemParams coordSysParams)
    {
      Utils.DrawCoordSystem(mainCanvas, coordSysParams, GetMinX(), GetMaxX(), GetMinY(), GetMaxY());

      List<Color> colors = new List<Color>();
      colors.Add(Colors.Black);
      colors.Add(Colors.Blue);
      colors.Add(Colors.Red);
      colors.Add(Colors.Green);
      colors.Add(Colors.Orange);
      colors.Add(Colors.Purple);
      colors.Add(Colors.GreenYellow);

      List<Brush> brushes= new List<Brush>();
      brushes.Add(Brushes.Black);
      brushes.Add(Brushes.Blue);
      brushes.Add(Brushes.Red);
      brushes.Add(Brushes.Green);
      brushes.Add(Brushes.Orange);
      brushes.Add(Brushes.Purple);
      brushes.Add(Brushes.GreenYellow);

      for (int i = 0; i < _multiFuncX.Elements.Length-1; i++)
      {
        // idemo po pojedinacnim funkcijama, odnosno njihovim redovim u matrici
        for (int j = 0; j < _multiFuncY.Rows; j++)
        {
          Line xAxis = new Line();
          if( j < brushes.Count)
            xAxis.Stroke = brushes[j];
          else
            xAxis.Stroke = Brushes.Black; // default color if j exceeds available colors

          double x1 = coordSysParams._centerX + _multiFuncX.Elements[i] * coordSysParams._scaleX;
          double y1 = coordSysParams._centerY - _multiFuncY.ElemAt(j, i) * coordSysParams._scaleY;
          double x2 = coordSysParams._centerX + _multiFuncX.Elements[i + 1] * coordSysParams._scaleX;
          double y2 = coordSysParams._centerY - _multiFuncY.ElemAt(j, i+1) * coordSysParams._scaleY;

          xAxis.X1 = x1;
          xAxis.Y1 = y1;
          xAxis.X2 = x2;
          xAxis.Y2 = y2;

          mainCanvas.Children.Add(xAxis);
        }
      }
    }
  };
}
