using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MML_ParticleVisualizer2D
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class ParticleVisualizer2D_MainWindow : Window
  {
    public ParticleVisualizer2D_MainWindow()
    {
      InitializeComponent();
    }

    public bool LoadData(string fileName)
    {
      return true;
    }
  }
}