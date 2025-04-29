using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;
using WPF3DHelperLib;

namespace MML_ParticleVisualizer3D
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class ParticleVisualizer3D_MainWindow : Window
  {
    readonly WorldCameraMouseHelper _helper = new WorldCameraMouseHelper();

    Model3DGroup _myModel3DGroup = new Model3DGroup();

    public ParticleVisualizer3D_MainWindow()
    {
      InitializeComponent();
    }
  }
}