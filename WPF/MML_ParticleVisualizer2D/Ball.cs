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
    private Vector2Cartesian _pos = new Vector2Cartesian();
    private Vector2Cartesian _speed = new Vector2Cartesian();

    public double _radius;
    public double _mass;
    private int _type;

    public Vector2Cartesian Position
    {
      get => _pos;
      set => _pos = value;
    }
    public Vector2Cartesian Velocity
    {
      get => _speed;
      set => _speed = value;
    }

    public double X
    {
      get => _pos.X;
      set => _pos.X = value;
    }

    public double Y
    {
      get => _pos.Y;
      set => _pos.Y = value;
    }

    public double Vx
    {
      get => _speed.X;
      set => _speed.X = value;
    }

    public double Vy
    {
      get => _speed.Y;
      set => _speed.Y = value;
    }
    public int Type { get => _type; set => _type = value; }
  }
}
