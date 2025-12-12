#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "MMLData.h"
#include <memory>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void LoadVectorField(std::unique_ptr<VectorField2D> field);
    void ClearVectorField();
    
    void SetArrowScale(double scale) { arrowScale_ = scale; update(); }
    void SetNormalizeVectors(bool normalize) { normalizeVectors_ = normalize; update(); }
    void SetShowMagnitudeColor(bool show) { showMagnitudeColor_ = show; update(); }
    
    double GetArrowScale() const { return arrowScale_; }
    bool GetNormalizeVectors() const { return normalizeVectors_; }
    bool GetShowMagnitudeColor() const { return showMagnitudeColor_; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void DrawGrid();
    void DrawAxes();
    void DrawVectors();
    void DrawArrow(double px, double py, double vx, double vy, double magnitude);
    QColor GetMagnitudeColor(double magnitude, double maxMagnitude) const;

    std::unique_ptr<VectorField2D> vectorField_;
    
    // View parameters
    double xMin_, xMax_;
    double yMin_, yMax_;
    double zoom_;
    double offsetX_;
    double offsetY_;
    
    // Arrow rendering options
    double arrowScale_;
    bool normalizeVectors_;
    bool showMagnitudeColor_;
    double maxMagnitude_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isPanning_;
};

#endif // GLWIDGET_H
