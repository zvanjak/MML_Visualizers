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

using MML;

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

        int lineNumber = 2;

        // read the balls attributes - name, radius, type
        for (int i = 0; i < numBalls; i++)
        {
          parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          
          if (parts.Length != 3)
            throw new Exception("Invalid ball attributes in SimData.txt");
          
          string name = parts[0];
          string color = parts[1];
          double radius = double.Parse(parts[2]);
          Ball ball = new Ball(name, color, radius);

          _balls.Add(ball);
        }

        // read number of steps
        parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length != 2)
          throw new Exception("Invalid number of steps in SimData.txt");
        int numSteps = int.Parse(parts[1]);

        // read the steps
        for (int i = 0; i < numSteps; i++)
        {
          // we expect "Step 0" together with timing (ie, additional double to specify T)
          parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
          if (parts.Length != 3)
            throw new Exception("Invalid number of steps in SimData.txt");

          if (parts[0] != "Step")
            throw new Exception("Invalid step in SimData.txt");
          if (parts[1] != i.ToString())
            throw new Exception("Invalid step number in SimData.txt");
          double T = double.Parse(parts[2]);

          // read the positions of the balls
          for (int j = 0; j < numBalls; j++)
          {
            parts = lines[lineNumber++].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length != 3)
              throw new Exception("Invalid ball position coords in SimData.txt, line number - " + lineNumber.ToString());

            int index = int.Parse(parts[0]);
            double x = double.Parse(parts[1]);
            double y = double.Parse(parts[2]);
            
            Vector2Cartesian pos = new Vector2Cartesian(x, y);
            
            _balls[j].AddPos(pos);
          }
        }
      }
      else
      {
      }

      return true;
    }
  }
}