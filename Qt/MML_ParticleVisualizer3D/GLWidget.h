#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    void SetSimulation(const LoadedParticleSimulation3D& sim);
    void SetCurrentStep(int step);
    void SetDisplayMode(DisplayMode mode);
    void SetParticleVisible(int index, bool visible);
    
    void ResetCamera();
    void LookAtCenter();
    
    LoadedParticleSimulation3D& GetSimulation() { return simulation_; }
    const LoadedParticleSimulation3D& GetSimulation() const { return simulation_; }

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void DrawSphere(const Point3D& center, double radius, const Color& color);
    void DrawAxes(bool extendBothDirections = false);
    void DrawBoundingBox();
    void DrawCoordinatePlanes();
    void DrawSemiTransparentPlane(float x1, float y1, float z1,
                                   float x2, float y2, float z2,
                                   float x3, float y3, float z3,
                                   float x4, float y4, float z4,
                                   const Color& color);
    void UpdateProjectionMatrix();
    
    LoadedParticleSimulation3D simulation_;
    int currentStep_;
    DisplayMode displayMode_;
    
    // Camera parameters
    QVector3D cameraPosition_;
    QVector3D lookAtPoint_;
    float cameraDistance_;
    float cameraRotationX_;
    float cameraRotationY_;
    
    // Initial camera state for reset
    QVector3D initialLookAtPoint_;
    float initialCameraDistance_;
    float initialCameraRotationX_;
    float initialCameraRotationY_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isRotating_;
    bool isPanning_;
    
    // Projection matrix
    QMatrix4x4 projectionMatrix_;
    QMatrix4x4 viewMatrix_;
    
    // Sphere rendering parameters
    static const int SPHERE_SLICES = 20;
    static const int SPHERE_STACKS = 20;
};

#endif // GLWIDGET_H
