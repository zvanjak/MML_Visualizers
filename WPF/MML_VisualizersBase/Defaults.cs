
using System.Windows.Media;

namespace MML_VisualizersBase
{
  public static class Defaults
  {
    static readonly List<SolidColorBrush> _brushes = new List<SolidColorBrush>() {
        new SolidColorBrush(Colors.Black),
        new SolidColorBrush(Colors.Blue),
        new SolidColorBrush(Colors.Red),
        new SolidColorBrush(Colors.Green),
        new SolidColorBrush(Colors.Orange),
        new SolidColorBrush(Colors.Purple),
        new SolidColorBrush(Colors.Brown),
        new SolidColorBrush(Colors.Cyan),
        new SolidColorBrush(Colors.Magenta),
        new SolidColorBrush(Colors.Gray),
        new SolidColorBrush(Colors.Yellow)
      };

    static public List<SolidColorBrush>  GetBrushList()
    {       
      return _brushes;
    }

    static readonly List<Color> _colors = new List<Color>()
    {
      Colors.Black,
      Colors.Blue,
      Colors.Red,
      Colors.Green,
      Colors.Orange,
      Colors.Purple,
      Colors.Brown,
      Colors.Cyan,
      Colors.Magenta,
      Colors.Gray,
      Colors.Yellow
    };

    static public List<Color> GetColorList()
    {
      return _colors;
    }
  }

}
