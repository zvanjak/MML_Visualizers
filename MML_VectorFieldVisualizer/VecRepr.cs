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
    public Vector3Cartesian Pos;
    public Vector3Cartesian Vec;

    public VecRepr(Vector3Cartesian pos, Vector3Cartesian vec)
    {
      Pos = pos;
      Vec = vec;
    }
  }
}
