#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <vector>
#include <memory>
#include <functional>
#include "MMLData.h"
#include "AxisTickCalculator.h"

// Callback for animation frame updates
using AnimationCallback = std::function<void()>;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget() override;

    // Curve management
    void AddCurve(std::unique_ptr<LoadedParamCurve2D> curve);
    void ClearCurves();
    void ResetView();
    
    std::vector<std::unique_ptr<LoadedParamCurve2D>>& GetCurves() { return curves_; }
    const std::vector<std::unique_ptr<LoadedParamCurve2D>>& GetCurves() const { return curves_; }
    
    // Get curve color (for legend)
    QColor GetCurveColor(int index) const;
    
    // Display settings
    bool IsGridVisible() const { return showGrid_; }
    void SetGridVisible(bool visible);
    
    bool AreLabelsVisible() const { return showLabels_; }
    void SetLabelsVisible(bool visible);
    
    bool IsAspectRatioPreserved() const { return preserveAspectRatio_; }
    void SetPreserveAspectRatio(bool preserve);
    
    // Animation controls
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
    
    // Visibility
    void SetCurveVisible(int index, bool visible);
    bool IsCurveVisible(int index) const;
    
    // Bounds info
    double GetDataMinX() const { return dataMinX_; }
    double GetDataMaxX() const { return dataMaxX_; }
    double GetDataMinY() const { return dataMinY_; }
    double GetDataMaxY() const { return dataMaxY_; }
    double GetDataMinT() const { return dataMinT_; }
    double GetDataMaxT() const { return dataMaxT_; }

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

private slots:
    void OnAnimationTimer();

private:
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels(QPainter& painter);
    void DrawCurve(const LoadedParamCurve2D& curve);
    void DrawAnimationMarkers(QPainter& painter);
    void CalculateBounds();
    void SetupProjection();
    
    std::vector<std::unique_ptr<LoadedParamCurve2D>> curves_;
    
    // Tick information
    AxisTickInfo xTickInfo_;
    AxisTickInfo yTickInfo_;
    
    // Data bounds
    double dataMinX_, dataMaxX_;
    double dataMinY_, dataMaxY_;
    double dataMinT_, dataMaxT_;
    
    // View parameters (nice bounds)
    double viewMinX_, viewMaxX_;
    double viewMinY_, viewMaxY_;
    
    // Actual displayed bounds
    double displayMinX_, displayMaxX_;
    double displayMinY_, displayMaxY_;
    
    // Default bounds for reset
    double defaultMinX_, defaultMaxX_;
    double defaultMinY_, defaultMaxY_;
    
    // Display settings
    bool showGrid_;
    bool showLabels_;
    bool preserveAspectRatio_;
    
    // Animation state
    QTimer* animationTimer_;
    bool animationRunning_;
    size_t currentAnimationFrame_;
    size_t maxAnimationFrames_;
    double animationSpeed_;  // Points per second
    AnimationCallback animationCallback_;
    
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
};

#endif // GL_WIDGET_H
