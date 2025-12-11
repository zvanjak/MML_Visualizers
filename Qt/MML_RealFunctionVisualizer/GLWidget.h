#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>
#include <memory>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget() override;

    void AddFunction(std::unique_ptr<LoadedRealFunction> func);
    void ClearFunctions();
    void ResetView();
    
    const std::vector<std::unique_ptr<LoadedRealFunction>>& GetFunctions() const {
        return functions_;
    }

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void DrawAxes();
    void DrawGrid();
    void DrawFunction(const LoadedRealFunction& func);
    void CalculateBounds();
    void UpdateProjection();
    
    std::vector<std::unique_ptr<LoadedRealFunction>> functions_;
    
    // View parameters
    double viewMinX_;
    double viewMaxX_;
    double viewMinY_;
    double viewMaxY_;
    double defaultMinX_;
    double defaultMaxX_;
    double defaultMinY_;
    double defaultMaxY_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isPanning_;
    
    int width_;
    int height_;
};

#endif // GL_WIDGET_H
