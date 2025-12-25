using System.Windows.Controls;
using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Defines the contract for loaded function data that can be visualized.
  /// </summary>
  /// <remarks>
  /// This interface provides a common abstraction for both single real functions
  /// and multi-dimensional function sets, enabling polymorphic rendering and
  /// bounds calculation in the visualizer.
  /// </remarks>
  public interface ILoadedFunction
  {
    /// <summary>
    /// Renders the function on the specified canvas using the given coordinate system parameters.
    /// </summary>
    /// <param name="canvas">The WPF canvas to draw on.</param>
    /// <param name="coordParams">The coordinate system transformation parameters.</param>
    void Draw(Canvas canvas, CoordSystemParams coordParams);

    /// <summary>
    /// Gets the minimum X value across all data points.
    /// </summary>
    /// <returns>The minimum X coordinate value.</returns>
    double GetMinX();

    /// <summary>
    /// Gets the maximum X value across all data points.
    /// </summary>
    /// <returns>The maximum X coordinate value.</returns>
    double GetMaxX();

    /// <summary>
    /// Gets the minimum Y value across all data points.
    /// </summary>
    /// <returns>The minimum Y coordinate value.</returns>
    double GetMinY();

    /// <summary>
    /// Gets the maximum Y value across all data points.
    /// </summary>
    /// <returns>The maximum Y coordinate value.</returns>
    double GetMaxY();

    /// <summary>
    /// Gets the total number of data points in the function.
    /// </summary>
    /// <returns>The number of (x, y) data points.</returns>
    int GetNumPoints();

    /// <summary>
    /// Gets the title/name of the function data set.
    /// </summary>
    string Title { get; }

    /// <summary>
    /// Gets the number of individual functions contained in this data set.
    /// </summary>
    /// <remarks>
    /// Returns 1 for single functions, or N for multi-function data sets
    /// where multiple y-values exist for each x-value.
    /// </remarks>
    int Dimension { get; }

    /// <summary>
    /// Gets the title of a specific function within a multi-function data set.
    /// </summary>
    /// <param name="index">The zero-based index of the function.</param>
    /// <returns>The title of the specified function.</returns>
    string GetFunctionTitle(int index);
  }
}
