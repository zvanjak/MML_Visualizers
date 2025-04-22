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
    List<Ball> _balls = new List<Ball>();

    public ParticleVisualizer2D_MainWindow()
    {
      InitializeComponent();

      LoadData("SimData.txt");
    }

    public bool LoadData(string fileName)
    {
      // load data about balls and their position from SimData.txt

      // get number of balls
      int numBalls = 0;
      string[] lines = System.IO.File.ReadAllLines(fileName);

      if (lines[0] == "PARTICLE_SIMULATION_DATA_2D")
      {
        // read the number of balls
        string[] parts = lines[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length != 2)
          throw new Exception("Invalid number of balls in SimData.txt");
        numBalls = int.Parse(parts[1]);

        // read the balls attributes - name, radius, type
      }
      else
      {
      }

      return true;
    }
  }
}