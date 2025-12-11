#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <memory>
#include <vector>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    void AddCurve(std::unique_ptr<LoadedParametricCurve3D> curve, const Color& color);
    void ClearCurves();
    void ResetCamera();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void DrawCurve(const LoadedParametricCurve3D* curve, const Color& color);
    void DrawAxes();
    void DrawGrid();
    void UpdateBounds();
    void SetupCamera();

    struct CurveData {
        std::unique_ptr<LoadedParametricCurve3D> curve;
        Color color;
    };

    std::vector<CurveData> curves_;
    
    // Camera parameters
    QMatrix4x4 projectionMatrix_;
    QMatrix4x4 viewMatrix_;
    float cameraDistance_;
    float cameraRotationX_;
    float cameraRotationY_;
    QVector3D cameraTarget_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isRotating_;
    bool isPanning_;
    
    // Scene bounds
    double xMin_, xMax_;
    double yMin_, yMax_;
    double zMin_, zMax_;
    double sceneRadius_;
};

#endif // GL_WIDGET_H
