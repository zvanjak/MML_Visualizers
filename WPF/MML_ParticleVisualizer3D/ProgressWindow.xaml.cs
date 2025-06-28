
using System.Windows;

namespace MML_ParticleVisualizer3D
{
  public partial class ProgressWindow : Window
  {
    public ProgressWindow()
    {
      InitializeComponent();
    }

    public void SetProgress(double percent)
    {
      ProgressBar.Value = percent;
    }
  }
}