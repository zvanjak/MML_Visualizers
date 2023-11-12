using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MML;

namespace MML_VectorFieldVisualizer
{
  public class VecRepr
  {
    public Vector3Cartesian _pos;
    public Vector3Cartesian _vec;

    public VecRepr(Vector3Cartesian pos, Vector3Cartesian vec)
    {
      _pos = pos;
      _vec = vec;
    }
  }
}
