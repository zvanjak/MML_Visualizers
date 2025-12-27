#define NOMINMAX
#include "GraphWidget.h"
#include <FL/Fl.H>
#include <sstream>
#include <iomanip>
#include <algorithm>

GraphWidget::GraphWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L) {
    coordParams_.preserveAspectRatio = true;  // Default true for parametric curves
}

GraphWidget::~GraphWidget() {
    StopAnimation();
}

void GraphWidget::AddCurve(std::unique_ptr<LoadedParametricCurve2D> curve) {
    curves_.push_back(std::move(curve));
    CalculateBounds();
    CalculateTicks();
    
    // Update max animation frames
    for (const auto& c : curves_) {
        maxAnimationFrames_ = std::max(maxAnimationFrames_, c->GetNumPoints());
    }
    
    redraw();
}

void GraphWidget::ClearCurves() {
    StopAnimation();
    curves_.clear();
    currentAnimationFrame_ = 0;
    maxAnimationFrames_ = 0;
    dataMinX_ = dataMaxX_ = dataMinY_ = dataMaxY_ = 0;
    dataMinT_ = dataMaxT_ = 0;
    redraw();
}

void GraphWidget::CalculateBounds() {
    if (curves_.empty()) {
        dataMinX_ = -1; dataMaxX_ = 1;
        dataMinY_ = -1; dataMaxY_ = 1;
        dataMinT_ = 0; dataMaxT_ = 1;
        return;
    }
    
    // Initialize with first curve's bounds
    dataMinX_ = curves_[0]->GetMinX();
    dataMaxX_ = curves_[0]->GetMaxX();
    dataMinY_ = curves_[0]->GetMinY();
    dataMaxY_ = curves_[0]->GetMaxY();
    dataMinT_ = curves_[0]->GetMinT();
    dataMaxT_ = curves_[0]->GetMaxT();
    
    // Expand bounds to include all curves
    for (const auto& curve : curves_) {
        dataMinX_ = std::min(dataMinX_, curve->GetMinX());
        dataMaxX_ = std::max(dataMaxX_, curve->GetMaxX());
        dataMinY_ = std::min(dataMinY_, curve->GetMinY());
        dataMaxY_ = std::max(dataMaxY_, curve->GetMaxY());
        dataMinT_ = std::min(dataMinT_, curve->GetMinT());
        dataMaxT_ = std::max(dataMaxT_, curve->GetMaxT());
    }
    
    // Handle equal bounds
    if (std::abs(dataMaxX_ - dataMinX_) < 1e-10) {
        dataMinX_ -= 1;
        dataMaxX_ += 1;
    }
    if (std::abs(dataMaxY_ - dataMinY_) < 1e-10) {
        dataMinY_ -= 1;
        dataMaxY_ += 1;
    }
}

void GraphWidget::CalculateTicks() {
    // Add 5% padding to data bounds
    double xPadding = (dataMaxX_ - dataMinX_) * 0.05;
    double yPadding = (dataMaxY_ - dataMinY_) * 0.05;
    
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        dataMinX_ - xPadding, dataMaxX_ + xPadding,
        dataMinY_ - yPadding, dataMaxY_ + yPadding,
        10, 8);
    
    coordParams_.xTickInfo = xTicks;
    coordParams_.yTickInfo = yTicks;
    coordParams_.xMin = xTicks.min;
    coordParams_.xMax = xTicks.max;
    coordParams_.yMin = yTicks.min;
    coordParams_.yMax = yTicks.max;
    coordParams_.tMin = dataMinT_;
    coordParams_.tMax = dataMaxT_;
}

void GraphWidget::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    coordParams_.UpdateFromWidget(w(), h());
}

void GraphWidget::draw() {
    // Update dimensions
    coordParams_.UpdateFromWidget(w(), h());
    
    // Draw background
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    
    // Set clipping to drawing area
    fl_push_clip(x() + coordParams_.drawX, y() + coordParams_.drawY, 
                 coordParams_.drawWidth, coordParams_.drawHeight);
    
    if (!curves_.empty()) {
        DrawCoordinateSystem();
        DrawCurves();
        
        if (animationRunning_ || currentAnimationFrame_ > 0) {
            DrawAnimationMarkers();
        }
    }
    
    fl_pop_clip();
    
    // Draw axis labels outside clip area
    if (!curves_.empty() && coordParams_.showAxisLabels) {
        DrawAxisLabels();
    }
}

void GraphWidget::DrawCoordinateSystem() {
    if (coordParams_.showGrid) {
        DrawGrid();
    }
    DrawAxes();
}

void GraphWidget::DrawGrid() {
    fl_color(220, 220, 220);
    fl_line_style(FL_SOLID, 1);
    
    // Vertical grid lines at X ticks
    for (const auto& tick : coordParams_.xTickInfo.ticks) {
        int screenX, screenY1, screenY2;
        coordParams_.WorldToScreen(tick.value, coordParams_.yTickInfo.min, screenX, screenY1);
        coordParams_.WorldToScreen(tick.value, coordParams_.yTickInfo.max, screenX, screenY2);
        fl_line(x() + screenX, y() + screenY1, x() + screenX, y() + screenY2);
    }
    
    // Horizontal grid lines at Y ticks
    for (const auto& tick : coordParams_.yTickInfo.ticks) {
        int screenX1, screenX2, screenY;
        coordParams_.WorldToScreen(coordParams_.xTickInfo.min, tick.value, screenX1, screenY);
        coordParams_.WorldToScreen(coordParams_.xTickInfo.max, tick.value, screenX2, screenY);
        fl_line(x() + screenX1, y() + screenY, x() + screenX2, y() + screenY);
    }
    
    fl_line_style(0);
}

void GraphWidget::DrawAxes() {
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 2);
    
    double xMin = coordParams_.xTickInfo.min;
    double xMax = coordParams_.xTickInfo.max;
    double yMin = coordParams_.yTickInfo.min;
    double yMax = coordParams_.yTickInfo.max;
    
    // X-axis (at y=0 if in range, otherwise at bottom)
    double xAxisY = (yMin <= 0 && yMax >= 0) ? 0 : yMin;
    int screenX1, screenY1, screenX2, screenY2;
    coordParams_.WorldToScreen(xMin, xAxisY, screenX1, screenY1);
    coordParams_.WorldToScreen(xMax, xAxisY, screenX2, screenY2);
    fl_line(x() + screenX1, y() + screenY1, x() + screenX2, y() + screenY2);
    
    // Y-axis (at x=0 if in range, otherwise at left)
    double yAxisX = (xMin <= 0 && xMax >= 0) ? 0 : xMin;
    coordParams_.WorldToScreen(yAxisX, yMin, screenX1, screenY1);
    coordParams_.WorldToScreen(yAxisX, yMax, screenX2, screenY2);
    fl_line(x() + screenX1, y() + screenY1, x() + screenX2, y() + screenY2);
    
    fl_line_style(0);
}

void GraphWidget::DrawAxisLabels() {
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA, 10);
    
    // X-axis labels
    for (const auto& tick : coordParams_.xTickInfo.ticks) {
        int screenX, screenY;
        coordParams_.WorldToScreen(tick.value, coordParams_.yTickInfo.min, screenX, screenY);
        
        int textWidth = static_cast<int>(fl_width(tick.label.c_str()));
        fl_draw(tick.label.c_str(), x() + screenX - textWidth/2, 
                y() + coordParams_.drawY + coordParams_.drawHeight + 15);
    }
    
    // Y-axis labels
    for (const auto& tick : coordParams_.yTickInfo.ticks) {
        int screenX, screenY;
        coordParams_.WorldToScreen(coordParams_.xTickInfo.min, tick.value, screenX, screenY);
        
        int textWidth = static_cast<int>(fl_width(tick.label.c_str()));
        fl_draw(tick.label.c_str(), x() + coordParams_.drawX - textWidth - 5, 
                y() + screenY + 4);
    }
}

void GraphWidget::DrawCurves() {
    for (const auto& curve : curves_) {
        if (curve && curve->IsVisible()) {
            // Draw curve with widget offset
            const auto& xVals = curve->GetXVals();
            const auto& yVals = curve->GetYVals();
            const auto& style = curve->GetStyle();
            CurveColor color = curve->GetColor();
            
            if (xVals.size() < 2) continue;
            
            fl_color(color.r, color.g, color.b);
            fl_line_style(FL_SOLID, static_cast<int>(style.lineWidth));
            
            // Draw as connected line segments
            for (size_t i = 0; i < xVals.size() - 1; ++i) {
                int screenX1, screenY1, screenX2, screenY2;
                coordParams_.WorldToScreen(xVals[i], yVals[i], screenX1, screenY1);
                coordParams_.WorldToScreen(xVals[i + 1], yVals[i + 1], screenX2, screenY2);
                
                fl_line(x() + screenX1, y() + screenY1, x() + screenX2, y() + screenY2);
            }
            
            fl_line_style(0);
        }
    }
}

void GraphWidget::DrawAnimationMarkers() {
    for (const auto& curve : curves_) {
        if (curve && curve->IsVisible() && currentAnimationFrame_ < curve->GetNumPoints()) {
            const auto& xVals = curve->GetXVals();
            const auto& yVals = curve->GetYVals();
            CurveColor color = curve->GetColor();
            
            int screenX, screenY;
            coordParams_.WorldToScreen(xVals[currentAnimationFrame_], 
                                       yVals[currentAnimationFrame_], 
                                       screenX, screenY);
            
            // Draw filled circle marker
            fl_color(color.r, color.g, color.b);
            fl_pie(x() + screenX - 6, y() + screenY - 6, 12, 12, 0, 360);
            
            // Draw black outline
            fl_color(FL_BLACK);
            fl_line_style(FL_SOLID, 1);
            fl_arc(x() + screenX - 6, y() + screenY - 6, 12, 12, 0, 360);
            fl_line_style(0);
        }
    }
}

// Animation methods
void GraphWidget::StartAnimation() {
    if (maxAnimationFrames_ == 0) return;
    
    animationRunning_ = true;
    double delay = 1.0 / animationSpeed_;
    Fl::add_timeout(delay, AnimationTimerCallback, this);
    redraw();
}

void GraphWidget::PauseAnimation() {
    animationRunning_ = false;
    Fl::remove_timeout(AnimationTimerCallback, this);
}

void GraphWidget::ResumeAnimation() {
    if (maxAnimationFrames_ == 0) return;
    
    animationRunning_ = true;
    double delay = 1.0 / animationSpeed_;
    Fl::add_timeout(delay, AnimationTimerCallback, this);
}

void GraphWidget::ResetAnimation() {
    currentAnimationFrame_ = 0;
    redraw();
}

void GraphWidget::StopAnimation() {
    animationRunning_ = false;
    Fl::remove_timeout(AnimationTimerCallback, this);
}

void GraphWidget::SetAnimationSpeed(double pointsPerSecond) {
    animationSpeed_ = std::max(0.1, pointsPerSecond);
}

void GraphWidget::AnimationTimerCallback(void* data) {
    GraphWidget* widget = static_cast<GraphWidget*>(data);
    widget->OnAnimationTimer();
}

void GraphWidget::OnAnimationTimer() {
    if (!animationRunning_) return;
    
    currentAnimationFrame_++;
    
    // Loop animation
    if (currentAnimationFrame_ >= maxAnimationFrames_) {
        currentAnimationFrame_ = 0;
    }
    
    redraw();
    
    if (animationFrameCallback_) {
        animationFrameCallback_();
    }
    
    // Schedule next frame
    double delay = 1.0 / animationSpeed_;
    Fl::add_timeout(delay, AnimationTimerCallback, this);
}

// Settings
void GraphWidget::SetPreserveAspectRatio(bool preserve) {
    coordParams_.preserveAspectRatio = preserve;
    redraw();
}

void GraphWidget::SetShowGrid(bool show) {
    coordParams_.showGrid = show;
    redraw();
}

void GraphWidget::SetShowAxisLabels(bool show) {
    coordParams_.showAxisLabels = show;
    redraw();
}

void GraphWidget::SetCurveVisible(int index, bool visible) {
    if (index >= 0 && index < static_cast<int>(curves_.size())) {
        curves_[index]->SetVisible(visible);
        redraw();
    }
}

bool GraphWidget::IsCurveVisible(int index) const {
    if (index >= 0 && index < static_cast<int>(curves_.size())) {
        return curves_[index]->IsVisible();
    }
    return false;
}

// LoadedParametricCurve2D Draw implementation
void LoadedParametricCurve2D::Draw(const CoordSystemParams& params) {
    if (xVals_.size() < 2) return;
    
    fl_color(color_.r, color_.g, color_.b);
    fl_line_style(FL_SOLID, static_cast<int>(style_.lineWidth));
    
    // Draw as connected line segments using fl_line (more reliable than fl_vertex)
    for (size_t i = 0; i < xVals_.size() - 1; ++i) {
        int screenX1, screenY1, screenX2, screenY2;
        params.WorldToScreen(xVals_[i], yVals_[i], screenX1, screenY1);
        params.WorldToScreen(xVals_[i + 1], yVals_[i + 1], screenX2, screenY2);
        
        fl_line(screenX1, screenY1, screenX2, screenY2);
    }
    
    fl_line_style(0);
    
    // Draw points if enabled
    if (style_.showPoints) {
        for (size_t i = 0; i < xVals_.size(); ++i) {
            int screenX, screenY;
            params.WorldToScreen(xVals_[i], yVals_[i], screenX, screenY);
            
            fl_color(color_.r, color_.g, color_.b);
            fl_pie(screenX - static_cast<int>(style_.pointRadius), 
                   screenY - static_cast<int>(style_.pointRadius),
                   static_cast<int>(style_.pointRadius * 2), 
                   static_cast<int>(style_.pointRadius * 2), 0, 360);
        }
    }
}

void LoadedParametricCurve2D::DrawMarker(const CoordSystemParams& params, size_t pointIndex) {
    if (pointIndex >= xVals_.size()) return;
    
    int screenX, screenY;
    params.WorldToScreen(xVals_[pointIndex], yVals_[pointIndex], screenX, screenY);
    
    // Draw filled circle marker
    fl_color(color_.r, color_.g, color_.b);
    fl_pie(screenX - 6, screenY - 6, 12, 12, 0, 360);
    
    // Draw black outline
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 1);
    fl_arc(screenX - 6, screenY - 6, 12, 12, 0, 360);
    fl_line_style(0);
}
