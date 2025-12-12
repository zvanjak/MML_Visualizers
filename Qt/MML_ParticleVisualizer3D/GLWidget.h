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
    void SetShowBoundingBox(bool show);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void DrawSphere(const Point3D& center, double radius, const Color& color);
    void DrawBoundingBox();
    void DrawAxes();
    void UpdateProjectionMatrix();
    
    LoadedParticleSimulation3D simulation_;
    int currentStep_;
    bool showBoundingBox_;
    
    // Camera parameters
    QVector3D cameraPosition_;
    QVector3D lookAtPoint_;
    float cameraDistance_;
    float cameraRotationX_;
    float cameraRotationY_;
    
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
