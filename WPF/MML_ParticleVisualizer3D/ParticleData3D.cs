using MML;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;

namespace MML_ParticleVisualizer3D
{
  class ParticleData3D
  {
    private List<Vector3Cartesian> _vecPos = new List<Vector3Cartesian>();

    private string _name;
    private string _color;
    private double _radius;

    public GeometryModel3D? _geomModel = null;

    public string Name { get => _name; set => _name = value; }
    public string Color { get => _color; set => _color = value; }
    public double Radius { get => _radius; set => _radius = value; }

    public Vector3Cartesian Pos(int i)
    {
      if (i < 0 || i >= _vecPos.Count)
        throw new IndexOutOfRangeException("Index out of range in Ball.Pos");
      return _vecPos[i];
    }
    public void AddPos(Vector3Cartesian pos)
    {
      _vecPos.Add(pos);
    }

    public ParticleData3D(string name, string color, double radius, GeometryModel3D geomModel)
    {
      _name = name;
      _color = color;
      _radius = radius;
      _geomModel = geomModel;
    }
  }
}
