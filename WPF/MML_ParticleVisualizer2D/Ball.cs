using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using MML;

namespace MML_ParticleVisualizer2D
{
  public class Ball
  {
    private List<Vector2Cartesian> _vecPos = new List<Vector2Cartesian>();

    private string _name;
    private double _radius;
    private int    _type;

    public int Type { get => _type; set => _type = value; }
    public double Radius { get => _radius; set => _radius = value; }
    public string Name { get => _name; set => _name = value; }

    public Vector2Cartesian Pos(int i)
    {
      if (i < 0 || i >= _vecPos.Count)
        throw new IndexOutOfRangeException("Index out of range in Ball.Pos");
      return _vecPos[i];
    }
    public void AddPos(Vector2Cartesian pos)
    {
      _vecPos.Add(pos);
    }

    public Ball(double radius, int type)
    {
      _radius = radius;
      _type = type;
    }
  }
}
