using System.Windows.Controls;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Defines the contract for loaded 2D parametric curve data that can be visualized.
  /// </summary>
  /// <remarks>
  /// <para>
  /// A parametric curve in 2D is defined by two functions x(t) and y(t) where t is the parameter.
  /// This interface provides access to the curve data and rendering capabilities.
  /// </para>
  /// <para>
  /// Implementations should store discrete samples of the curve at various parameter values
  /// and provide methods for querying bounds and rendering the curve.
  /// </para>
  /// </remarks>
  public interface ILoadedCurve2D
  {
    /// <summary>
    /// Renders the parametric curve on the specified canvas.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    void Draw(Canvas canvas, CoordSystemParams coordParams);

    /// <summary>
    /// Gets the minimum X coordinate value across all curve points.
    /// </summary>
    /// <returns>The minimum X value.</returns>
    double GetMinX();

    /// <summary>
    /// Gets the maximum X coordinate value across all curve points.
    /// </summary>
    /// <returns>The maximum X value.</returns>
    double GetMaxX();

    /// <summary>
    /// Gets the minimum Y coordinate value across all curve points.
    /// </summary>
    /// <returns>The minimum Y value.</returns>
    double GetMinY();

    /// <summary>
    /// Gets the maximum Y coordinate value across all curve points.
    /// </summary>
    /// <returns>The maximum Y value.</returns>
    double GetMaxY();

    /// <summary>
    /// Gets the minimum parameter value (start of curve).
    /// </summary>
    /// <returns>The minimum t parameter value.</returns>
    double GetMinT();

    /// <summary>
    /// Gets the maximum parameter value (end of curve).
    /// </summary>
    /// <returns>The maximum t parameter value.</returns>
    double GetMaxT();

    /// <summary>
    /// Gets the number of sampled points on the curve.
    /// </summary>
    /// <returns>The number of (t, x, y) data points.</returns>
    int GetNumPoints();

    /// <summary>
    /// Gets the title/name of the parametric curve.
    /// </summary>
    string Title { get; }
  }
}
