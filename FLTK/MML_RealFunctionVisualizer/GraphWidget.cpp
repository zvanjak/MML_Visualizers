#define NOMINMAX
#include "GraphWidget.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// Define color palette (similar to WPF version)
const std::vector<Color> GraphWidget::colors_ = {
    Color(0, 0, 0),       // Black
    Color(0, 0, 255),     // Blue
    Color(255, 0, 0),     // Red
    Color(0, 128, 0),     // Green
    Color(255, 165, 0),   // Orange
    Color(128, 0, 128),   // Purple
    Color(165, 42, 42),   // Brown
    Color(0, 255, 255),   // Cyan
    Color(255, 0, 255),   // Magenta
    Color(128, 128, 128), // Gray
    Color(255, 255, 0)    // Yellow
};

GraphWidget::GraphWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L), 
      dataXMin_(-10), dataXMax_(10), dataYMin_(-10), dataYMax_(10) {
}

void GraphWidget::AddFunction(std::unique_ptr<LoadedFunction> func) {
    functions_.push_back(std::move(func));
}

void GraphWidget::ClearFunctions() {
    functions_.clear();
}

void GraphWidget::CalculateDataBounds() {
    if (functions_.empty()) {
        dataXMin_ = -10;
        dataXMax_ = 10;
        dataYMin_ = -10;
        dataYMax_ = 10;
        coordParams_.numPoints = 0;
        return;
    }
    
    // Find visible functions and calculate bounds
    bool firstVisible = true;
    dataXMin_ = 0;
    dataXMax_ = 1;
    dataYMin_ = 0;
    dataYMax_ = 1;
    
    for (const auto& func : functions_) {
        if (!func->IsVisible()) continue;
        
        if (firstVisible) {
            dataXMin_ = func->GetMinX();
            dataXMax_ = func->GetMaxX();
            dataYMin_ = func->GetMinY();
            dataYMax_ = func->GetMaxY();
            firstVisible = false;
        } else {
            dataXMin_ = std::min(dataXMin_, func->GetMinX());
            dataXMax_ = std::max(dataXMax_, func->GetMaxX());
            dataYMin_ = std::min(dataYMin_, func->GetMinY());
            dataYMax_ = std::max(dataYMax_, func->GetMaxY());
        }
    }
    
    // Use first function's point count
    if (!functions_.empty()) {
        coordParams_.numPoints = functions_[0]->GetNumPoints();
    }
    
    // Handle edge cases
    if (firstVisible) {
        // No visible functions
        dataXMin_ = -10;
        dataXMax_ = 10;
        dataYMin_ = -10;
        dataYMax_ = 10;
    }
    
    // Handle degenerate ranges
    if (std::abs(dataXMax_ - dataXMin_) < 1e-10) {
        double padding = std::abs(dataXMin_) * 0.1;
        if (padding < 1e-10) padding = 1.0;
        dataXMin_ -= padding;
        dataXMax_ += padding;
    }
    if (std::abs(dataYMax_ - dataYMin_) < 1e-10) {
        double padding = std::abs(dataYMin_) * 0.1;
        if (padding < 1e-10) padding = 1.0;
        dataYMin_ -= padding;
        dataYMax_ += padding;
    }
}

void GraphWidget::InitializeCoordParams() {
    CalculateDataBounds();
    
    // Calculate nice tick values
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        dataXMin_, dataXMax_, dataYMin_, dataYMax_, 10, 8);
    
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    // Use nice bounds for coordinate system
    coordParams_.xMin = dataXMin_;
    coordParams_.xMax = dataXMax_;
    coordParams_.yMin = dataYMin_;
    coordParams_.yMax = dataYMax_;
    coordParams_.niceXMin = xTicks.min;
    coordParams_.niceXMax = xTicks.max;
    coordParams_.niceYMin = yTicks.min;
    coordParams_.niceYMax = yTicks.max;
    
    coordParams_.windowWidth = w();
    coordParams_.windowHeight = h();
    
    // Calculate scales with 5% margin
    double marginFactor = 0.9;
    
    double rangeX = xTicks.max - xTicks.min;
    double rangeY = yTicks.max - yTicks.min;
    
    if (rangeX < 1e-10) rangeX = 1.0;
    if (rangeY < 1e-10) rangeY = 1.0;
    
    double scaleX = (coordParams_.windowWidth * marginFactor) / rangeX;
    double scaleY = (coordParams_.windowHeight * marginFactor) / rangeY;
    
    if (coordParams_.preserveAspectRatio) {
        // Use uniform scale
        double uniformScale = std::min(scaleX, scaleY);
        coordParams_.scaleX = uniformScale;
        coordParams_.scaleY = uniformScale;
    } else {
        coordParams_.scaleX = scaleX;
        coordParams_.scaleY = scaleY;
    }
    
    // Calculate center offset
    double midPointX = (xTicks.min + xTicks.max) / 2.0;
    double midPointY = (yTicks.min + yTicks.max) / 2.0;
    
    coordParams_.centerX = coordParams_.windowWidth / 2.0 - midPointX * coordParams_.scaleX;
    coordParams_.centerY = coordParams_.windowHeight / 2.0 + midPointY * coordParams_.scaleY;
}

void GraphWidget::WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
    screenX = static_cast<int>(x() + coordParams_.centerX + worldX * coordParams_.scaleX);
    screenY = static_cast<int>(y() + coordParams_.centerY - worldY * coordParams_.scaleY);
}

void GraphWidget::ScreenToWorld(int screenX, int screenY, double& worldX, double& worldY) const {
    worldX = (screenX - x() - coordParams_.centerX) / coordParams_.scaleX;
    worldY = (coordParams_.centerY - (screenY - y())) / coordParams_.scaleY;
}

void GraphWidget::DrawGrid() {
    fl_color(style_.gridColor.r, style_.gridColor.g, style_.gridColor.b);
    fl_line_style(FL_SOLID, static_cast<int>(style_.gridThickness));
    
    // Vertical grid lines at X tick positions
    for (const auto& tick : xTickInfo_.ticks) {
        int screenX, screenY1, screenY2;
        WorldToScreen(tick.value, yTickInfo_.min, screenX, screenY1);
        WorldToScreen(tick.value, yTickInfo_.max, screenX, screenY2);
        fl_line(screenX, screenY1, screenX, screenY2);
    }
    
    // Horizontal grid lines at Y tick positions
    for (const auto& tick : yTickInfo_.ticks) {
        int screenX1, screenX2, screenY;
        WorldToScreen(xTickInfo_.min, tick.value, screenX1, screenY);
        WorldToScreen(xTickInfo_.max, tick.value, screenX2, screenY);
        fl_line(screenX1, screenY, screenX2, screenY);
    }
    
    fl_line_style(FL_SOLID, 1);
}

void GraphWidget::DrawAxes() {
    fl_color(style_.axisColor.r, style_.axisColor.g, style_.axisColor.b);
    fl_line_style(FL_SOLID, static_cast<int>(style_.axisThickness));
    
    // Determine X-axis Y position (at y=0 if in range, otherwise at edge)
    double xAxisY = 0;
    if (yTickInfo_.min > 0) xAxisY = yTickInfo_.min;
    else if (yTickInfo_.max < 0) xAxisY = yTickInfo_.max;
    
    // Determine Y-axis X position (at x=0 if in range, otherwise at edge)
    double yAxisX = 0;
    if (xTickInfo_.min > 0) yAxisX = xTickInfo_.min;
    else if (xTickInfo_.max < 0) yAxisX = xTickInfo_.max;
    
    // Draw X-axis
    int xAxisStartX, xAxisStartY, xAxisEndX, xAxisEndY;
    WorldToScreen(xTickInfo_.min, xAxisY, xAxisStartX, xAxisStartY);
    WorldToScreen(xTickInfo_.max, xAxisY, xAxisEndX, xAxisEndY);
    fl_line(xAxisStartX, xAxisStartY, xAxisEndX, xAxisEndY);
    
    // Draw Y-axis
    int yAxisStartX, yAxisStartY, yAxisEndX, yAxisEndY;
    WorldToScreen(yAxisX, yTickInfo_.min, yAxisStartX, yAxisStartY);
    WorldToScreen(yAxisX, yTickInfo_.max, yAxisEndX, yAxisEndY);
    fl_line(yAxisStartX, yAxisStartY, yAxisEndX, yAxisEndY);
    
    fl_line_style(FL_SOLID, 1);
}

void GraphWidget::DrawAxisTicks() {
    fl_color(style_.tickColor.r, style_.tickColor.g, style_.tickColor.b);
    fl_line_style(FL_SOLID, 1);
    
    // Determine axis positions
    double xAxisY = 0;
    if (yTickInfo_.min > 0) xAxisY = yTickInfo_.min;
    else if (yTickInfo_.max < 0) xAxisY = yTickInfo_.max;
    
    double yAxisX = 0;
    if (xTickInfo_.min > 0) yAxisX = xTickInfo_.min;
    else if (xTickInfo_.max < 0) yAxisX = xTickInfo_.max;
    
    int tickLen = static_cast<int>(style_.tickLength);
    
    // X-axis ticks
    for (const auto& tick : xTickInfo_.ticks) {
        int screenX, screenY;
        WorldToScreen(tick.value, xAxisY, screenX, screenY);
        fl_line(screenX, screenY - tickLen, screenX, screenY + tickLen);
    }
    
    // Y-axis ticks
    for (const auto& tick : yTickInfo_.ticks) {
        int screenX, screenY;
        WorldToScreen(yAxisX, tick.value, screenX, screenY);
        fl_line(screenX - tickLen, screenY, screenX + tickLen, screenY);
    }
}

void GraphWidget::DrawAxisLabels() {
    fl_color(style_.labelColor.r, style_.labelColor.g, style_.labelColor.b);
    fl_font(FL_HELVETICA, style_.labelFontSize);
    
    // Determine axis positions
    double xAxisY = 0;
    if (yTickInfo_.min > 0) xAxisY = yTickInfo_.min;
    else if (yTickInfo_.max < 0) xAxisY = yTickInfo_.max;
    
    double yAxisX = 0;
    if (xTickInfo_.min > 0) yAxisX = xTickInfo_.min;
    else if (xTickInfo_.max < 0) yAxisX = xTickInfo_.max;
    
    // X-axis labels
    for (const auto& tick : xTickInfo_.ticks) {
        int screenX, screenY;
        WorldToScreen(tick.value, xAxisY, screenX, screenY);
        
        // Measure text width for centering
        int textWidth = 0, textHeight = 0;
        fl_measure(tick.label.c_str(), textWidth, textHeight);
        
        // Position label below the axis
        int labelX = screenX - textWidth / 2;
        int labelY = screenY + static_cast<int>(style_.tickLength) + textHeight + 2;
        
        fl_draw(tick.label.c_str(), labelX, labelY);
    }
    
    // Y-axis labels
    for (const auto& tick : yTickInfo_.ticks) {
        int screenX, screenY;
        WorldToScreen(yAxisX, tick.value, screenX, screenY);
        
        // Measure text width for right-alignment
        int textWidth = 0, textHeight = 0;
        fl_measure(tick.label.c_str(), textWidth, textHeight);
        
        // Position label to the left of the axis
        int labelX = screenX - static_cast<int>(style_.tickLength) - textWidth - 5;
        int labelY = screenY + textHeight / 3;
        
        fl_draw(tick.label.c_str(), labelX, labelY);
    }
}

void GraphWidget::DrawCoordinateSystem() {
    if (style_.showGrid) {
        DrawGrid();
    }
    DrawAxes();
    DrawAxisTicks();
    if (style_.showAxisLabels) {
        DrawAxisLabels();
    }
}

void GraphWidget::RecalculateAndRedraw() {
    redraw();
    if (redrawCallback_) {
        redrawCallback_();
    }
}

void GraphWidget::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    InitializeCoordParams();
}

void GraphWidget::draw() {
    // Clear background
    fl_color(255, 255, 255); // White background
    fl_rectf(x(), y(), w(), h());
    
    InitializeCoordParams();
    DrawCoordinateSystem();
    
    // Draw all visible functions
    for (auto& func : functions_) {
        if (func->IsVisible()) {
            func->Draw(this, coordParams_);
        }
    }
    
    // Draw border
    fl_color(0, 0, 0);
    fl_rect(x(), y(), w(), h());
}

// Implementation of Draw methods for loaded functions

void SingleLoadedFunction::Draw(GraphWidget* widget, const CoordSystemParams& params) {
    if (!IsVisible()) return;
    
    const auto& xVals = GetXVals();
    const auto& yVals = GetYVals();
    
    if (xVals.size() < 2) return;
    
    Color color = GraphWidget::GetColor(GetIndex());
    fl_color(color.r, color.g, color.b);
    fl_line_style(FL_SOLID, static_cast<int>(drawStyle_.lineThickness));
    
    for (size_t i = 0; i < xVals.size() - 1; ++i) {
        int x1, y1, x2, y2;
        widget->WorldToScreen(xVals[i], yVals[i], x1, y1);
        widget->WorldToScreen(xVals[i + 1], yVals[i + 1], x2, y2);
        fl_line(x1, y1, x2, y2);
    }
    
    fl_line_style(FL_SOLID, 1);
}

void MultiLoadedFunction::Draw(GraphWidget* widget, const CoordSystemParams& params) {
    const auto& xVals = GetXVals();
    const auto& yVals = GetYVals();
    
    if (xVals.size() < 2) return;
    
    for (size_t funcIndex = 0; funcIndex < yVals.size(); ++funcIndex) {
        // Check visibility for this sub-function
        if (!IsFunctionVisible(static_cast<int>(funcIndex))) continue;
        
        Color color = GraphWidget::GetColor(static_cast<int>(funcIndex));
        fl_color(color.r, color.g, color.b);
        fl_line_style(FL_SOLID, 2);
        
        for (size_t i = 0; i < xVals.size() - 1; ++i) {
            int x1, y1, x2, y2;
            widget->WorldToScreen(xVals[i], yVals[funcIndex][i], x1, y1);
            widget->WorldToScreen(xVals[i + 1], yVals[funcIndex][i + 1], x2, y2);
            fl_line(x1, y1, x2, y2);
        }
    }
    
    fl_line_style(FL_SOLID, 1);
}
