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
    
    // Arrow rendering settings
    void SetMagnitudeScale(double scale) { magnitudeScale_ = scale; update(); }
    void SetArrowSize(int size) { arrowSize_ = size; update(); }
    void SetNormalizeVectors(bool normalize) { normalizeVectors_ = normalize; update(); }
    void SetColorByMagnitude(bool enabled) { colorByMagnitude_ = enabled; update(); }
    void SetPreserveAspectRatio(bool preserve) { preserveAspectRatio_ = preserve; update(); }
    void SetArrowColorIndex(int index) { arrowColorIndex_ = index; update(); }
    
    double GetMagnitudeScale() const { return magnitudeScale_; }
    int GetArrowSize() const { return arrowSize_; }
    bool GetNormalizeVectors() const { return normalizeVectors_; }
    bool GetColorByMagnitude() const { return colorByMagnitude_; }
    bool GetPreserveAspectRatio() const { return preserveAspectRatio_; }
    int GetArrowColorIndex() const { return arrowColorIndex_; }
    double GetOptimalScale() const { return optimalScale_; }
    
    // Statistics access
    const VectorField2D* GetVectorField() const { return vectorField_.get(); }

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
    QColor GetMagnitudeColor(double magnitude) const;
    QColor GetSolidArrowColor() const;

    std::unique_ptr<VectorField2D> vectorField_;
    
    // View parameters
    double xMin_, xMax_;
    double yMin_, yMax_;
    double zoom_;
    double offsetX_;
    double offsetY_;
    
    // Arrow rendering options
    double magnitudeScale_;      // User scale (0.1-5.0), default 1.0
    double optimalScale_;        // Auto-calculated
    int arrowSize_;              // Arrowhead size (2-20), default 8
    bool normalizeVectors_;
    bool colorByMagnitude_;
    bool preserveAspectRatio_;
    int arrowColorIndex_;        // 0=Black, 1=Blue, 2=Red, 3=Green, 4=Orange, 5=Purple
    
    double minMagnitude_;
    double maxMagnitude_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isPanning_;
};

#endif // GLWIDGET_H
