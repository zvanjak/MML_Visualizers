#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void LoadSimulation(const SimulationData& data);
    void ClearSimulation();
    
    // Animation controls
    void Play();
    void Pause();
    void Stop();
    void StepForward();
    void StepBackward();
    void SetTimestep(int timestep);
    void SetAnimationSpeed(int fps);
    
    bool IsPlaying() const { return isPlaying_; }
    int GetCurrentTimestep() const { return currentTimestep_; }
    int GetNumTimesteps() const { return numTimesteps_; }
    
signals:
    void TimestepChanged(int timestep);
    void AnimationFinished();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void OnAnimationTimer();

private:
    void DrawGrid();
    void DrawAxes();
    void DrawBalls();
    void DrawBall(double centerX, double centerY, double radius, const std::string& colorName);
    void UpdateView();
    
    QColor GetColorFromName(const std::string& colorName) const;

    // Simulation data
    SimulationData simData_;
    int numTimesteps_;
    int currentTimestep_;
    
    // Animation
    QTimer* animTimer_;
    bool isPlaying_;
    int animationFPS_;
    
    // View parameters
    double simWidth_;
    double simHeight_;
    double scaleX_;
    double scaleY_;
    double offsetX_;
    double offsetY_;
    double zoom_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isPanning_;
};

#endif // GLWIDGET_H
