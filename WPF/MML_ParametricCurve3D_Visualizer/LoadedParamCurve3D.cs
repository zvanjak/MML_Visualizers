using MML;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Media.Media3D;

namespace MML_ParametricCurve3D_Visualizer
{
  /// <summary>
  /// Represents a 3D parametric curve loaded from a data file.
  /// </summary>
  /// <remarks>
  /// <para>
  /// A parametric curve in 3D is defined by three functions:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>x = x(t) - the X coordinate as a function of parameter t</description></item>
  ///   <item><description>y = y(t) - the Y coordinate as a function of parameter t</description></item>
  ///   <item><description>z = z(t) - the Z coordinate as a function of parameter t</description></item>
  /// </list>
  /// <para>
  /// Examples of 3D parametric curves include:
  /// </para>
  /// <list type="bullet">
  ///   <item><description>Helix: x(t) = cos(t), y(t) = sin(t), z(t) = t</description></item>
  ///   <item><description>Toroidal spiral: x(t) = (R + r·cos(nt))·cos(t), y(t) = (R + r·cos(nt))·sin(t), z(t) = r·sin(nt)</description></item>
  ///   <item><description>Space curves from physics: Lorenz attractor, planetary orbits</description></item>
  /// </list>
  /// </remarks>
  public class LoadedCurve
  {
    private readonly List<double> _tVals = new List<double>();

    /// <summary>
    /// Gets or sets the display title of the parametric curve.
    /// </summary>
    public string Title { get; set; } = "";

    /// <summary>
    /// Gets or sets whether this curve should be rendered.
    /// When false, the curve is hidden from the visualization.
    /// </summary>
    public bool IsVisible { get; set; } = true;

    /// <summary>
    /// Gets or sets the 3D model representing this curve in the scene.
    /// </summary>
    public GeometryModel3D? CurveModel { get; set; }

    /// <summary>
    /// Gets the list of 3D points that define the curve trace.
    /// </summary>
    public List<Vector3Cartesian> CurveTrace { get; } = new List<Vector3Cartesian>();

    /// <summary>
    /// Gets the parameter values (t) as a read-only collection.
    /// </summary>
    public IReadOnlyList<double> TValues => _tVals;

    /// <summary>
    /// Adds a sampled point to the parametric curve.
    /// </summary>
    /// <param name="t">The parameter value at this sample.</param>
    /// <param name="x">The X coordinate value x(t).</param>
    /// <param name="y">The Y coordinate value y(t).</param>
    /// <param name="z">The Z coordinate value z(t).</param>
    public void AddPoint(double t, double x, double y, double z)
    {
      _tVals.Add(t);
      CurveTrace.Add(new Vector3Cartesian(x, y, z));
    }

    /// <summary>
    /// Gets the number of sampled points on the curve.
    /// </summary>
    public int GetNumPoints() => CurveTrace.Count;

    /// <summary>
    /// Gets the minimum parameter value (start of curve).
    /// </summary>
    public double GetMinT() => _tVals.Count > 0 ? _tVals.Min() : 0;

    /// <summary>
    /// Gets the maximum parameter value (end of curve).
    /// </summary>
    public double GetMaxT() => _tVals.Count > 0 ? _tVals.Max() : 0;

    /// <summary>
    /// Gets the minimum X coordinate value across all curve points.
    /// </summary>
    public double GetMinX() => CurveTrace.Count > 0 ? CurveTrace.Min(v => v.X) : 0;

    /// <summary>
    /// Gets the maximum X coordinate value across all curve points.
    /// </summary>
    public double GetMaxX() => CurveTrace.Count > 0 ? CurveTrace.Max(v => v.X) : 0;

    /// <summary>
    /// Gets the minimum Y coordinate value across all curve points.
    /// </summary>
    public double GetMinY() => CurveTrace.Count > 0 ? CurveTrace.Min(v => v.Y) : 0;

    /// <summary>
    /// Gets the maximum Y coordinate value across all curve points.
    /// </summary>
    public double GetMaxY() => CurveTrace.Count > 0 ? CurveTrace.Max(v => v.Y) : 0;

    /// <summary>
    /// Gets the minimum Z coordinate value across all curve points.
    /// </summary>
    public double GetMinZ() => CurveTrace.Count > 0 ? CurveTrace.Min(v => v.Z) : 0;

    /// <summary>
    /// Gets the maximum Z coordinate value across all curve points.
    /// </summary>
    public double GetMaxZ() => CurveTrace.Count > 0 ? CurveTrace.Max(v => v.Z) : 0;
  }

  /// <summary>
  /// Helper class for animated sphere markers in 3D visualization.
  /// </summary>
  public class Sphere
  {
    /// <summary>Gets or sets the X position of the sphere.</summary>
    public double X { get; set; }

    /// <summary>Gets or sets the Y position of the sphere.</summary>
    public double Y { get; set; }

    /// <summary>Gets or sets the Z position of the sphere.</summary>
    public double Z { get; set; }

    /// <summary>Gets or sets the reference to the 3D geometry model for transforms.</summary>
    public GeometryModel3D? RefGeomModel { get; set; }
  }
}
