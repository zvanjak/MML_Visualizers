using System.Windows.Controls;

using WPF3DHelperLib;

namespace MML_RealFunctionVisualizer
{
  public interface ILoadedFunction
  {
    void Draw(Canvas canvas, CoordSystemParams coordParams);
    double GetMinX();
    double GetMaxX();
    double GetMinY();
    double GetMaxY();
    int GetNumPoints();
    string Title { get; }
    int Dimension { get; }
    string GetFunctionTitle(int index);
  }
}
