using System.Windows.Controls;
using WPF3DHelperLib;

namespace MML_ParametricCurve2D_Visualizer
{
  /// <summary>
  /// Interface for loaded parametric curves.
  /// </summary>
  public interface ILoadedCurve2D
  {
    void Draw(Canvas canvas, CoordSystemParams coordParams);
    double GetMinX();
    double GetMaxX();
    double GetMinY();
    double GetMaxY();
    double GetMinT();
    double GetMaxT();
    int GetNumPoints();
    string Title { get; }
  }
}
