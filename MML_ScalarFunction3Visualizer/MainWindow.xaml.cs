using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;

using MML;
using WPF3DHelperLib;

namespace MML_ScalarFunction3Visualizer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    PerspectiveCamera myPCamera = new PerspectiveCamera();

    Point3D _cameraPos = new Point3D(180, 80, 150);
    Point3D _lookToPos = new Point3D(0, 0, 0);

    Point3D _startCameraPosRButtonClick;

    private bool _bLButtonDown = false;
    private bool _bRightButtonDown = false;

    private Point _lastMousePos;
    private Point _startMouseRButtonClick;

    public MainWindow()
    {
      InitializeComponent();
    }

    private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      _bLButtonDown = true;
      _lastMousePos = e.GetPosition(this);
    }

    private void Window_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      _bLButtonDown = false;
    }

    private void Window_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
      _bRightButtonDown = true;
      _startMouseRButtonClick = e.GetPosition(this);

      _startCameraPosRButtonClick = _cameraPos;
    }

    private void Window_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
    {
      _bRightButtonDown = false;
    }

    private void Window_MouseMove(object sender, MouseEventArgs e)
    {
      if (_bLButtonDown)
      {
        // morati ćemo uzeti u obzir i smjer gledanja na kraju!
        var newPos = e.GetPosition(this);
        var diffX = (newPos.X - _lastMousePos.X) / 100.0;
        var diffY = (newPos.Y - _lastMousePos.Y) / 100.0;

        // uzeti smjer gledanja kao normalu, i kreirati horizontalni x-y koord sustav
        // koji određuje koliki su u stvari dx i dy
        _cameraPos = new Point3D(_cameraPos.X + diffX, _cameraPos.Y - diffY, _cameraPos.Z);
        _lookToPos = new Point3D(_lookToPos.X + diffX, _lookToPos.Y - diffY, _lookToPos.Z);

        myPCamera.Position = _cameraPos;

        myViewport3D.InvalidateVisual();
      }

      if (_bRightButtonDown)
      {
        // za početak, samo ćemo se micati lijevo desno
        double diffX = e.GetPosition(this).X - _startMouseRButtonClick.X;
        double diffY = e.GetPosition(this).Y - _startMouseRButtonClick.Y;

        Debug.WriteLine("Diff {0} - {1}", diffX, diffY);

        // znači, moramo zarotirati točku kamere, OKO točke gledanja
        double angleX = diffX / 5.0 * Math.PI / 180.0;
        double angleY = diffY / 5.0 * Math.PI / 180.0;

        Debug.WriteLine("Angle {0} - {1}", angleX, angleY);

        // treba oduzeti _lookAtPos, da translatiramo origin
        Point3D cam = _startCameraPosRButtonClick;
        cam.X -= _lookToPos.X;
        cam.Y -= _lookToPos.Y;

        // transformiramo u sferne koordinate
        Point3Cartesian camPnt = new Point3Cartesian(cam.X, cam.Y, cam.Z);
        Point3Spherical outPnt = camPnt.GetSpherical();

        //Debug.WriteLine("Polar {0}  Elevation {1}", polar, elevation);

        outPnt.Phi += angleX;
        outPnt.Theta += angleY;
        if (outPnt.Theta < 0.0)
          outPnt.Theta = 0.05;

        Point3Cartesian newCamPos = outPnt.GetCartesian();

        cam.X = newCamPos.X;
        cam.Y = newCamPos.Y;
        cam.Z = newCamPos.Z;

        cam.X += _lookToPos.X;
        cam.Y += _lookToPos.Y;

        _cameraPos = cam;

        myPCamera.Position = _cameraPos;

        Debug.WriteLine("{0}  Elevation - {1}", _cameraPos.ToString(), outPnt.Theta);

        // treba ažurirati i LookDirection!!!
        myPCamera.LookDirection = Utils.getFrom2Points(_cameraPos, _lookToPos);

        myViewport3D.InvalidateVisual();
      }
    }

    private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      // mijenjamo poziciju kamere da se ili približi ili udalji od točke u koju gledamo
      Vector3D dir = Utils.getFrom2Points(_cameraPos, _lookToPos);

      _cameraPos = _cameraPos + (e.Delta / 10.0) * dir;

      myPCamera.Position = _cameraPos;

      myViewport3D.InvalidateVisual();
    }
  }
}
