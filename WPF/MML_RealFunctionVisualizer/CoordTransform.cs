using System.Windows;
using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  /// <summary>
  /// Utility class for coordinate transformations between world and screen coordinates.
  /// </summary>
  public static class CoordTransform
  {
    /// <summary>
    /// Transforms world coordinates to screen coordinates.
    /// </summary>
    public static Point WorldToScreen(double worldX, double worldY, CoordSystemParams coordParams)
    {
      double screenX = coordParams._centerX + worldX * coordParams._scaleX;
      double screenY = coordParams._centerY - worldY * coordParams._scaleY;
      return new Point(screenX, screenY);
    }

    /// <summary>
    /// Transforms screen coordinates to world coordinates.
    /// </summary>
    public static Point ScreenToWorld(double screenX, double screenY, CoordSystemParams coordParams)
    {
      double worldX = (screenX - coordParams._centerX) / coordParams._scaleX;
      double worldY = (coordParams._centerY - screenY) / coordParams._scaleY;
      return new Point(worldX, worldY);
    }

    /// <summary>
    /// Gets the screen X coordinate for a world X value.
    /// </summary>
    public static double WorldToScreenX(double worldX, CoordSystemParams coordParams)
    {
      return coordParams._centerX + worldX * coordParams._scaleX;
    }

    /// <summary>
    /// Gets the screen Y coordinate for a world Y value.
    /// </summary>
    public static double WorldToScreenY(double worldY, CoordSystemParams coordParams)
    {
      return coordParams._centerY - worldY * coordParams._scaleY;
    }
  }
}
