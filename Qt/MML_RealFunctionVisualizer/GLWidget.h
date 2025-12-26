#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFont>
#include <QPainter>
#include <vector>
#include <memory>
#include <functional>
#include "MMLData.h"
#include "AxisTickCalculator.h"

// Callback for when visibility changes
using VisibilityChangedCallback = std::function<void()>;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget() override;

    // Function management
    void AddFunction(std::unique_ptr<LoadedFunction> func);
    void ClearFunctions();
    void ResetView();
    
    std::vector<std::unique_ptr<LoadedFunction>>& GetFunctions() { return functions_; }
    const std::vector<std::unique_ptr<LoadedFunction>>& GetFunctions() const { return functions_; }

    // Display settings
    bool IsGridVisible() const { return showGrid_; }
    void SetGridVisible(bool visible);
    
    bool AreLabelsVisible() const { return showLabels_; }
    void SetLabelsVisible(bool visible);
    
    bool IsAspectRatioPreserved() const { return preserveAspectRatio_; }
    void SetPreserveAspectRatio(bool preserve);
    
    // Callback
    void SetVisibilityChangedCallback(VisibilityChangedCallback cb) { visibilityCallback_ = cb; }
    
    // Force bounds recalculation
    void RecalculateBounds();

signals:
    void boundsChanged();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels(QPainter& painter);
    void DrawSingleFunction(const LoadedRealFunction& func);
    void DrawMultiFunction(const MultiLoadedFunction& func);
    void CalculateBounds();
    void SetupProjection();
    
    std::vector<std::unique_ptr<LoadedFunction>> functions_;
    
    // Tick information
    AxisTickInfo xTickInfo_;
    AxisTickInfo yTickInfo_;
    
    // View parameters (nice bounds)
    double viewMinX_;
    double viewMaxX_;
    double viewMinY_;
    double viewMaxY_;
    
    // Actual displayed bounds (may differ when aspect ratio is preserved)
    double displayMinX_;
    double displayMaxX_;
    double displayMinY_;
    double displayMaxY_;
    
    // Default bounds for reset
    double defaultMinX_;
    double defaultMaxX_;
    double defaultMinY_;
    double defaultMaxY_;
    
    // Display settings
    bool showGrid_;
    bool showLabels_;
    bool preserveAspectRatio_;
    
    // Mouse interaction
    QPoint lastMousePos_;
    bool isPanning_;
    
    // OpenGL initialization flag
    bool glInitialized_;
    
    int width_;
    int height_;
    
    // Margin for labels (in pixels)
    static constexpr int MARGIN_LEFT = 60;
    static constexpr int MARGIN_BOTTOM = 40;
    static constexpr int MARGIN_TOP = 20;
    static constexpr int MARGIN_RIGHT = 20;
    
    VisibilityChangedCallback visibilityCallback_;
};

#endif // GL_WIDGET_H
