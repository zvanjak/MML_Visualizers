using System.Windows;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
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
  }
}
