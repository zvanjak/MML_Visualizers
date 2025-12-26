#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include "AxisTickCalculator.h"
#include <vector>
#include <memory>
#include <cmath>
#include <functional>

// Callback for animation frame updates
using AnimationCallback = std::function<void()>;

class GraphWidget : public Fl_Widget {
private:
    std::vector<std::unique_ptr<LoadedParametricCurve2D>> curves_;
    CoordSystemParams coordParams_;
    
    // Animation state
    bool animationRunning_ = false;
    size_t currentAnimationFrame_ = 0;
    size_t maxAnimationFrames_ = 0;
    double animationSpeed_ = 10.0;  // Points per second
    AnimationCallback animationFrameCallback_;
    
    // Bounds
    double dataMinX_ = 0, dataMaxX_ = 1;
    double dataMinY_ = 0, dataMaxY_ = 1;
    double dataMinT_ = 0, dataMaxT_ = 1;
    
    void CalculateBounds();
    void CalculateTicks();
    void DrawCoordinateSystem();
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels();
    void DrawCurves();
    void DrawAnimationMarkers();
    
    // Animation timer callback
    static void AnimationTimerCallback(void* data);
    void OnAnimationTimer();
    
public:
    GraphWidget(int X, int Y, int W, int H, const char* L = nullptr);
    ~GraphWidget();
    
    void draw() override;
    
    // Curve management
    void AddCurve(std::unique_ptr<LoadedParametricCurve2D> curve);
    void ClearCurves();
    const std::vector<std::unique_ptr<LoadedParametricCurve2D>>& GetCurves() const { return curves_; }
    
    // Coordinate system access
    CoordSystemParams& GetCoordParams() { return coordParams_; }
    const CoordSystemParams& GetCoordParams() const { return coordParams_; }
    
    // Settings
    void SetPreserveAspectRatio(bool preserve);
    bool GetPreserveAspectRatio() const { return coordParams_.preserveAspectRatio; }
    
    void SetShowGrid(bool show);
    bool GetShowGrid() const { return coordParams_.showGrid; }
    
    void SetShowAxisLabels(bool show);
    bool GetShowAxisLabels() const { return coordParams_.showAxisLabels; }
    
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
    
    void SetAnimationFrameCallback(AnimationCallback callback) { animationFrameCallback_ = callback; }
    
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
    
    // Color helper
    static CurveColor GetColor(int index) {
        return GetColorByIndex(index);
    }
};

#endif // GRAPH_WIDGET_H
