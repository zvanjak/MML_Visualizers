#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QTimer>
#include <memory>
#include <vector>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    // Curve management
    void AddCurve(std::unique_ptr<LoadedParametricCurve3D> curve);
    void ClearCurves();
    void ResetCamera();
    
    const std::vector<std::unique_ptr<LoadedParametricCurve3D>>& GetCurves() const { return curves_; }
    
    // Visibility
    void SetCurveVisible(int index, bool visible);
    bool IsCurveVisible(int index) const;
    
    // Line width
    void SetLineWidth(float width);
    float GetLineWidth() const { return lineWidth_; }
    void IncreaseLineWidth() { SetLineWidth(lineWidth_ * 1.1f); }
    void DecreaseLineWidth() { SetLineWidth(lineWidth_ * 0.9f); }
    
    // Animation
    void StartAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    void ResetAnimation();
    void StopAnimation();
    
    bool IsAnimationRunning() const { return animationRunning_; }
    size_t GetCurrentAnimationFrame() const { return currentAnimationFrame_; }
    size_t GetMaxAnimationFrames() const { return maxAnimationFrames_; }
    
    void SetAnimationSpeed(double pointsPerSecond);
    double GetAnimationSpeed() const { return animationSpeed_; }
    
    void SetAnimationFrameCallback(AnimationCallback callback) { animationCallback_ = callback; }
    
    // Scene info
    double GetSceneRadius() const { return sceneRadius_; }

signals:
    void boundsChanged();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void OnAnimationTimer();

private:
    void DrawCurve(const LoadedParametricCurve3D* curve);
    void DrawAxes();
    void DrawGrid();
    void DrawAnimationMarkers();
    void DrawSphere(const Point3D& center, float radius, const Color& color);
    void UpdateBounds();
    void SetupCamera();

    std::vector<std::unique_ptr<LoadedParametricCurve3D>> curves_;
    
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
    
    // Line width
    float lineWidth_;
    
    // Animation
    QTimer* animationTimer_;
    bool animationRunning_;
    size_t currentAnimationFrame_;
    size_t maxAnimationFrames_;
    double animationSpeed_;
    AnimationCallback animationCallback_;
};

#endif // GL_WIDGET_H
