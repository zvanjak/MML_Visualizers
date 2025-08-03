using MML;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MML_ParametricCurve3D_Visualizer
{
  class LoadedCurve
  {
    public string? _title;

    public List<Vector3Cartesian> _curveTrace = new List<Vector3Cartesian>();

    public double getMinX()
    {
      double minX = 0.0;
      if (_curveTrace.Count > 0)
        minX = _curveTrace.Min(v => v.X);
      return minX;
    }
    public double getMaxX()
    {
      double minX = 0.0;
      if (_curveTrace.Count > 0)
        minX = _curveTrace.Max(v => v.X);
      return minX;
    }

    public double getMinY()
    {
      double minY = 0.0;
      if (_curveTrace.Count > 0)
        minY = _curveTrace.Min(v => v.Y);
      return minY;
    }
    public double getMaxY()
    {
      double minY = 0.0;
      if (_curveTrace.Count > 0)
        minY = _curveTrace.Max(v => v.Y);
      return minY;
    }

    public double getMinZ()
    {
      double minZ = 0.0;
      if (_curveTrace.Count > 0)
        minZ = _curveTrace.Min(v => v.Z);
      return minZ;
    }
    public double getMaxZ()
    {
      double maxZ = 0.0;
      if (_curveTrace.Count > 0)
        maxZ = _curveTrace.Max(v => v.Z);
      return maxZ;
    }
  }
}
