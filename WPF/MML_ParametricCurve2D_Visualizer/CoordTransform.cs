using System.Windows;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Provides coordinate transformation utilities between mathematical world coordinates
  /// and WPF screen (pixel) coordinates.
  /// </summary>
  /// <remarks>
  /// <para>
  /// This class handles the transformation between two coordinate systems:
  /// </para>
  /// <list type="bullet">
  ///   <item>
  ///     <term>World coordinates</term>
  ///     <description>Mathematical coordinates where Y increases upward and values can be any real number.</description>
  ///   </item>
  ///   <item>
  ///     <term>Screen coordinates</term>
  ///     <description>WPF pixel coordinates where Y increases downward and origin is at top-left.</description>
  ///   </item>
  /// </list>
  /// <para>
  /// The transformation uses scale factors and center offsets stored in <see cref="CoordSystemParams"/>
  /// to properly position and scale the visualization.
  /// </para>
  /// </remarks>
  public static class CoordTransform
  {
    /// <summary>
    /// Transforms a point from world coordinates to screen coordinates.
    /// </summary>
    /// <param name="worldX">The X coordinate in world space.</param>
    /// <param name="worldY">The Y coordinate in world space.</param>
    /// <param name="coordParams">The coordinate system parameters containing scale and offset values.</param>
    /// <returns>A <see cref="Point"/> representing the screen position in pixels.</returns>
    /// <remarks>
    /// The Y coordinate is inverted because screen coordinates have Y increasing downward,
    /// while mathematical coordinates have Y increasing upward.
    /// </remarks>
    public static Point WorldToScreen(double worldX, double worldY, CoordSystemParams coordParams)
    {
      double screenX = coordParams._centerX + worldX * coordParams._scaleX;
      double screenY = coordParams._centerY - worldY * coordParams._scaleY;
      return new Point(screenX, screenY);
    }

    /// <summary>
    /// Transforms a point from screen coordinates to world coordinates.
    /// </summary>
    /// <param name="screenX">The X coordinate in screen pixels.</param>
    /// <param name="screenY">The Y coordinate in screen pixels.</param>
    /// <param name="coordParams">The coordinate system parameters containing scale and offset values.</param>
    /// <returns>A <see cref="Point"/> representing the world position.</returns>
    /// <remarks>
    /// This is the inverse transformation of <see cref="WorldToScreen"/>, useful for
    /// converting mouse positions to mathematical coordinates.
    /// </remarks>
    public static Point ScreenToWorld(double screenX, double screenY, CoordSystemParams coordParams)
    {
      double worldX = (screenX - coordParams._centerX) / coordParams._scaleX;
      double worldY = (coordParams._centerY - screenY) / coordParams._scaleY;
      return new Point(worldX, worldY);
    }
  }
}
