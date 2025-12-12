#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColor>
#include <vector>
#include <memory>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget() override;

    void AddCurve(std::unique_ptr<LoadedParamCurve2D> curve);
    void ClearCurves();
    void ResetView();
    
    const std::vector<std::unique_ptr<LoadedParamCurve2D>>& GetCurves() const {
        return curves_;
    }
    
    // Get curve colors (for legend)
    QColor GetCurveColor(int index) const;

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
    void DrawCurve(const LoadedParamCurve2D& curve, const QColor& color);
    void CalculateBounds();
    void UpdateProjection();
    
    std::vector<std::unique_ptr<LoadedParamCurve2D>> curves_;
    std::vector<QColor> curveColors_;
    
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
