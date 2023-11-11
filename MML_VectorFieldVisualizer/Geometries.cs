using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.Xml;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;

namespace MathVisualizer
{
  public class Geometries
  {
    public static MeshGeometry3D CreateParallelepiped(Point3D inCenter, double lengthX, double lengthY, double lengthZ)
    {
      MeshGeometry3D mesh = new MeshGeometry3D();

      Point3D p1 = new Point3D(inCenter.X - lengthX / 2, inCenter.Y + lengthY / 2, inCenter.Z + lengthZ / 2);
      Point3D p2 = new Point3D(inCenter.X + lengthX / 2, inCenter.Y + lengthY / 2, inCenter.Z + lengthZ / 2);
      Point3D p3 = new Point3D(inCenter.X + lengthX / 2, inCenter.Y + lengthY / 2, inCenter.Z - lengthZ / 2);
      Point3D p4 = new Point3D(inCenter.X - lengthX / 2, inCenter.Y + lengthY / 2, inCenter.Z - lengthZ / 2);
      Point3D p5 = new Point3D(inCenter.X - lengthX / 2, inCenter.Y - lengthY / 2, inCenter.Z + lengthZ / 2);
      Point3D p6 = new Point3D(inCenter.X + lengthX / 2, inCenter.Y - lengthY / 2, inCenter.Z + lengthZ / 2);
      Point3D p7 = new Point3D(inCenter.X + lengthX / 2, inCenter.Y - lengthY / 2, inCenter.Z - lengthZ / 2);
      Point3D p8 = new Point3D(inCenter.X - lengthX / 2, inCenter.Y - lengthY / 2, inCenter.Z - lengthZ / 2);

      mesh.Positions.Add(p1);
      mesh.Positions.Add(p2);
      mesh.Positions.Add(p3);
      mesh.Positions.Add(p4);
      mesh.Positions.Add(p5);
      mesh.Positions.Add(p6);
      mesh.Positions.Add(p7);
      mesh.Positions.Add(p8);

      // gornja ploha
      mesh.TriangleIndices.Add(0);
      mesh.TriangleIndices.Add(1);
      mesh.TriangleIndices.Add(2);

      mesh.TriangleIndices.Add(0);
      mesh.TriangleIndices.Add(2);
      mesh.TriangleIndices.Add(3);

      // donja ploha
      mesh.TriangleIndices.Add(4);
      mesh.TriangleIndices.Add(6);
      mesh.TriangleIndices.Add(5);

      mesh.TriangleIndices.Add(4);
      mesh.TriangleIndices.Add(7);
      mesh.TriangleIndices.Add(6);

      // srednja 1 ploha
      mesh.TriangleIndices.Add(0);
      mesh.TriangleIndices.Add(5);
      mesh.TriangleIndices.Add(1);

      mesh.TriangleIndices.Add(0);
      mesh.TriangleIndices.Add(4);
      mesh.TriangleIndices.Add(5);

      // srednja 2 ploha
      mesh.TriangleIndices.Add(1);
      mesh.TriangleIndices.Add(5);
      mesh.TriangleIndices.Add(6);

      mesh.TriangleIndices.Add(1);
      mesh.TriangleIndices.Add(6);
      mesh.TriangleIndices.Add(2);

      // srednja 3 ploha
      mesh.TriangleIndices.Add(2);
      mesh.TriangleIndices.Add(6);
      mesh.TriangleIndices.Add(7);

      mesh.TriangleIndices.Add(2);
      mesh.TriangleIndices.Add(7);
      mesh.TriangleIndices.Add(3);

      // srednja 4 ploha
      mesh.TriangleIndices.Add(3);
      mesh.TriangleIndices.Add(7);
      mesh.TriangleIndices.Add(4);

      mesh.TriangleIndices.Add(3);
      mesh.TriangleIndices.Add(4);
      mesh.TriangleIndices.Add(0);

      return mesh;
    }

    public static MeshGeometry3D CreateValjak(double baseRadius, double height, int numBaseDivs, int numHeightDivs)
    {
      MeshGeometry3D mesh = new MeshGeometry3D();

      // add base points
      for (int i = 0; i < numBaseDivs; i++)
      {
        double angle = 2 * Math.PI * i / numBaseDivs;
        double x = baseRadius * Math.Cos(angle);
        double y = baseRadius * Math.Sin(angle);

        Point3D p = new Point3D(x, y, 0);
        mesh.Positions.Add(p);
      }

      double segmentHeight = height / numHeightDivs;
      for (int h = 1; h <= numHeightDivs; h++)
      {
        // add layer points
        for (int i = 0; i < numBaseDivs; i++)
        {
          double angle = 2 * Math.PI * i / numBaseDivs;
          double x = baseRadius * Math.Cos(angle);
          double y = baseRadius * Math.Sin(angle);

          Point3D p = new Point3D(x, y, h * segmentHeight);
          mesh.Positions.Add(p);
        }

        // sad dodati triangle za layer
        for (int i = 0; i < numBaseDivs; i++)
        {
          int ind1 = (h - 1) * numBaseDivs + i;
          int ind2 = (h - 1) * numBaseDivs + (i + 1) % numBaseDivs;
          int ind3 = h * numBaseDivs + i;

          mesh.TriangleIndices.Add(ind1);
          mesh.TriangleIndices.Add(ind2);
          mesh.TriangleIndices.Add(ind3);

          ind1 = (h - 1) * numBaseDivs + (i + 1) % numBaseDivs;
          ind2 = h * numBaseDivs + (i + 1) % numBaseDivs;
          ind3 = h * numBaseDivs + i;

          mesh.TriangleIndices.Add(ind1);
          mesh.TriangleIndices.Add(ind2);
          mesh.TriangleIndices.Add(ind3);
        }
      }

      // add base and top triangles
      // first, add center points on base and top
      Point3D pBase = new Point3D(0, 0, 0);
      Point3D pTop = new Point3D(0, 0, height);
      mesh.Positions.Add(pBase);
      mesh.Positions.Add(pTop);

      int baseCenterInd = (numHeightDivs + 1) * numBaseDivs;
      int topCenterInd = (numHeightDivs + 1) * numBaseDivs + 1;

      for (int i = 0; i < numBaseDivs; i++)
      {
        // base
        int ind1 = i;
        int ind2 = baseCenterInd;
        int ind3 = (i + 1) % numBaseDivs;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);

        // top
        ind1 = numHeightDivs * numBaseDivs + i;
        ind2 = numHeightDivs * numBaseDivs + (i + 1) % numBaseDivs;
        ind3 = topCenterInd;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);
      }

      return mesh;
    }

    public static MeshGeometry3D CreateVectorArrow(double baseRadius, double height, int numBaseDivs, int numHeightDivs, double topAdditionalRadius, double topAdditionalHeight)
    {
      MeshGeometry3D mesh = new MeshGeometry3D();

      // add base points
      for (int i = 0; i < numBaseDivs; i++)
      {
        double angle = 2 * Math.PI * i / numBaseDivs;
        double x = baseRadius * Math.Cos(angle);
        double y = baseRadius * Math.Sin(angle);

        Point3D p = new Point3D(x, y, 0);
        mesh.Positions.Add(p);
      }

      double segmentHeight = height / numHeightDivs;
      for (int h = 1; h <= numHeightDivs; h++)
      {
        // add layer points
        for (int i = 0; i < numBaseDivs; i++)
        {
          double angle = 2 * Math.PI * i / numBaseDivs;
          double x = baseRadius * Math.Cos(angle);
          double y = baseRadius * Math.Sin(angle);

          Point3D p = new Point3D(x, y, h * segmentHeight);
          mesh.Positions.Add(p);
        }

        // sad dodati triangle za layer
        for (int i = 0; i < numBaseDivs; i++)
        {
          int ind1 = (h - 1) * numBaseDivs + i;
          int ind2 = (h - 1) * numBaseDivs + (i + 1) % numBaseDivs;
          int ind3 = h * numBaseDivs + i;

          mesh.TriangleIndices.Add(ind1);
          mesh.TriangleIndices.Add(ind2);
          mesh.TriangleIndices.Add(ind3);

          ind1 = (h - 1) * numBaseDivs + (i + 1) % numBaseDivs;
          ind2 = h * numBaseDivs + (i + 1) % numBaseDivs;
          ind3 = h * numBaseDivs + i;

          mesh.TriangleIndices.Add(ind1);
          mesh.TriangleIndices.Add(ind2);
          mesh.TriangleIndices.Add(ind3);
        }
      }

      // add base triangle
      // first, add center point on base 
      Point3D pBase = new Point3D(0, 0, 0);
      mesh.Positions.Add(pBase);

      int baseCenterInd = (numHeightDivs + 1) * numBaseDivs;

      for (int i = 0; i < numBaseDivs; i++)
      {
        // base
        int ind1 = i;
        int ind2 = baseCenterInd;
        int ind3 = (i + 1) % numBaseDivs;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);
      }

      // adding vector arrow
      Point3D pTop = new Point3D(0, 0, height + topAdditionalHeight);
      mesh.Positions.Add(pTop);
      int topCenterInd = (numHeightDivs + 1) * numBaseDivs + 1;

      // sad dodati obrub na vrhu
      // najprije dodavanje tocaka
      for (int i = 0; i < numBaseDivs; i++)
      {
        double angle = 2 * Math.PI * i / numBaseDivs;
        double x = (baseRadius + topAdditionalRadius) * Math.Cos(angle);
        double y = (baseRadius + topAdditionalRadius) * Math.Sin(angle);

        Point3D p = new Point3D(x, y, height);
        mesh.Positions.Add(p);
      }

      // a onda i triangles, najprije obrub gornji
      for (int i = 0; i < numBaseDivs; i++)
      {
        // top
        int ind1 = 2 + (numHeightDivs + 1) * numBaseDivs + i;
        int ind2 = 2 + (numHeightDivs + 1) * numBaseDivs + (i + 1) % numBaseDivs;
        int ind3 = topCenterInd;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);
      }

      // a onda i obrobu s donje strane
      for (int i = 0; i < numBaseDivs; i++)
      {
        int ind1 = 2 + (numHeightDivs + 1) * numBaseDivs + i;
        int ind2 = numHeightDivs * numBaseDivs + i;
        int ind3 = 2 + (numHeightDivs + 1) * numBaseDivs + (i + 1) % numBaseDivs;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);

        ind1 = 2 + (numHeightDivs + 1) * numBaseDivs + (i + 1) % numBaseDivs;
        ind2 = numHeightDivs * numBaseDivs + i;
        ind3 = numHeightDivs * numBaseDivs + (i + 1) % numBaseDivs;

        mesh.TriangleIndices.Add(ind1);
        mesh.TriangleIndices.Add(ind2);
        mesh.TriangleIndices.Add(ind3);
      }

      return mesh;
    }

    public static MeshGeometry3D CreateSphere(Point3D center, double radius)
    {
      MeshGeometry3D mesh = new MeshGeometry3D();

      int N = 10;
      int M = 10;    // koliko stripova po paralelama imamo

      double angleLatitudeStep = 2 * Math.PI / N;
      double angleLongitudeStep = Math.PI / M;

      Point3D northPole = new Point3D(center.X, center.Y, center.Z); northPole.Z += radius;
      int northPoleInd = 0;
      mesh.Positions.Add(northPole);

      // točke na ekvatoru
      // dodati dvije paralele
      double angleLon = 0;
      for (int j = 1; j < M; j++)
      {
        angleLon = angleLongitudeStep * j;
        double circleRadius = radius * Math.Sin(angleLon);
        double z = radius * Math.Cos(angleLon);

        double angleLat = 0;
        for (int i = 0; i < N; i++)
        {
          angleLat = angleLatitudeStep * i;
          Point3D p = new Point3D(center.X + circleRadius * Math.Cos(angleLat), center.Y + circleRadius * Math.Sin(angleLat), center.Z + z);

          mesh.Positions.Add(p);
        }
      }

      Point3D southPole = new Point3D(center.X, center.Y, center.Z); southPole.Z -= radius;
      int southPoleInd = N * (M - 1) + 1;
      mesh.Positions.Add(southPole);

      // dodajemo od North pole do ekvatora
      for (int i = 1; i <= N; i++)
      {
        mesh.TriangleIndices.Add(northPoleInd);
        mesh.TriangleIndices.Add(i);
        if (i < N)
          mesh.TriangleIndices.Add(i + 1);
        else
          mesh.TriangleIndices.Add(1);
      }

      // sad idemo dodati stripove između
      for (int j = 1; j < M - 1; j++)
      {
        int upperParallelStartInd = 1 + N * (j - 1);
        int lowerParallelStartInd = 1 + N * j;

        for (int i = 0; i < N; i++)
        {
          // dodajemo "lijevi" triangle četverokuta
          mesh.TriangleIndices.Add(upperParallelStartInd + i);
          mesh.TriangleIndices.Add(lowerParallelStartInd + i);
          if (i < N - 1)
            mesh.TriangleIndices.Add(lowerParallelStartInd + i + 1);
          else
            mesh.TriangleIndices.Add(lowerParallelStartInd);

          mesh.TriangleIndices.Add(upperParallelStartInd + i);
          if (i < N - 1)
            mesh.TriangleIndices.Add(lowerParallelStartInd + i + 1);
          else
            mesh.TriangleIndices.Add(lowerParallelStartInd);
          if (i < N - 1)
            mesh.TriangleIndices.Add(upperParallelStartInd + i + 1);
          else
            mesh.TriangleIndices.Add(upperParallelStartInd);


        }
      }

      // sad donja polusfera
      int lastParallelStartInd = 1 + N * (M - 2);
      for (int i = 0; i < N; i++)
      {
        mesh.TriangleIndices.Add(southPoleInd);
        if (i < N - 1)
          mesh.TriangleIndices.Add(lastParallelStartInd + i + 1);
        else
          mesh.TriangleIndices.Add(lastParallelStartInd);

        mesh.TriangleIndices.Add(lastParallelStartInd + i);
      }

      return mesh;
    }

    public static MeshGeometry3D CreateDvostrukiStozac(Point3D center, double radius, double height)
    {
      MeshGeometry3D mesh = new MeshGeometry3D();

      int N = 20;
      double angleStep = 2 * Math.PI / N;

      Point3D northPole = center; northPole.Z += height;
      int northPoleInd = 0;
      mesh.Positions.Add(northPole);

      // točke na ekvatoru
      double angle = 0;
      for (int i = 0; i < N; i++)
      {
        angle = angleStep * i;
        Point3D p = new Point3D(radius * Math.Cos(angle), radius * Math.Sin(angle), 0);

        mesh.Positions.Add(p);
      }

      Point3D southPole = center; southPole.Z -= height;
      int southPoleInd = N + 1;
      mesh.Positions.Add(southPole);

      // dodajemo od North pole do ekvatora
      for (int i = 1; i <= N; i++)
      {
        mesh.TriangleIndices.Add(northPoleInd);
        mesh.TriangleIndices.Add(i);
        if (i < N)
          mesh.TriangleIndices.Add(i + 1);
        else
          mesh.TriangleIndices.Add(1);
      }

      // sad donja polusfera
      for (int i = 1; i <= N; i++)
      {
        mesh.TriangleIndices.Add(southPoleInd);
        if (i < N)
          mesh.TriangleIndices.Add(i + 1);
        else
          mesh.TriangleIndices.Add(1);

        mesh.TriangleIndices.Add(i);
      }

      return mesh;
    }
  }
}
