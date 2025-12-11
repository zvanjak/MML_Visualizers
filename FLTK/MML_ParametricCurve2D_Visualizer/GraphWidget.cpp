#define NOMINMAX
#include "GraphWidget.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// Define color palette
const std::vector<Color> GraphWidget::colors_ = {
    Color(0, 0, 0),       // Black
    Color(0, 0, 255),     // Blue
    Color(255, 0, 0),     // Red
    Color(0, 128, 0),     // Green
    Color(255, 165, 0),   // Orange
    Color(128, 0, 128),   // Purple
    Color(0, 255, 255),   // Cyan
    Color(255, 0, 255),   // Magenta
};

GraphWidget::GraphWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L) {
}

void GraphWidget::AddCurve(std::unique_ptr<LoadedParametricCurve2D> curve) {
    curves_.push_back(std::move(curve));
    InitializeCoordParams();
}

void GraphWidget::ClearCurves() {
    curves_.clear();
    InitializeCoordParams();
}

void GraphWidget::InitializeCoordParams() {
    if (curves_.empty()) {
        coordParams_.xMin = -1;
        coordParams_.xMax = 1;
        coordParams_.yMin = -1;
        coordParams_.yMax = 1;
        coordParams_.numPoints = 0;
    } else {
        // Initialize with first curve's bounds
        coordParams_.xMin = curves_[0]->GetMinX();
        coordParams_.xMax = curves_[0]->GetMaxX();
        coordParams_.yMin = curves_[0]->GetMinY();
        coordParams_.yMax = curves_[0]->GetMaxY();
        coordParams_.numPoints = curves_[0]->GetNumPoints();
        
        // Expand bounds to include all curves
        for (const auto& curve : curves_) {
            coordParams_.xMin = std::min(coordParams_.xMin, curve->GetMinX());
            coordParams_.xMax = std::max(coordParams_.xMax, curve->GetMaxX());
            coordParams_.yMin = std::min(coordParams_.yMin, curve->GetMinY());
            coordParams_.yMax = std::max(coordParams_.yMax, curve->GetMaxY());
        }
        
        // Add 5% padding
        double xPadding = (coordParams_.xMax - coordParams_.xMin) * 0.05;
        double yPadding = (coordParams_.yMax - coordParams_.yMin) * 0.05;
        coordParams_.xMin -= xPadding;
        coordParams_.xMax += xPadding;
        coordParams_.yMin -= yPadding;
        coordParams_.yMax += yPadding;
    }
    
    // Calculate scaling and centering
    coordParams_.windowWidth = w();
    coordParams_.windowHeight = h();
    
    double xRange = coordParams_.xMax - coordParams_.xMin;
    double yRange = coordParams_.yMax - coordParams_.yMin;
    
    if (xRange <= 0) xRange = 1;
    if (yRange <= 0) yRange = 1;
    
    coordParams_.scaleX = (coordParams_.windowWidth * 0.9) / xRange;
    coordParams_.scaleY = (coordParams_.windowHeight * 0.9) / yRange;
    
    double midPointX = (coordParams_.xMin + coordParams_.xMax) / 2;
    double midPointY = (coordParams_.yMin + coordParams_.yMax) / 2;
    
    coordParams_.centerX = coordParams_.windowWidth / 2 - midPointX * coordParams_.scaleX;
    coordParams_.centerY = coordParams_.windowHeight / 2 + midPointY * coordParams_.scaleY;
}

void GraphWidget::WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
    screenX = static_cast<int>(coordParams_.centerX + worldX * coordParams_.scaleX);
    screenY = static_cast<int>(coordParams_.centerY - worldY * coordParams_.scaleY);
    
    // Add widget position offset
    screenX += x();
    screenY += y();
}

std::string GraphWidget::FormatNumber(double value) const {
    std::ostringstream oss;
    if (std::abs(value) < 0.001 || std::abs(value) > 1000) {
        oss << std::scientific << std::setprecision(2) << value;
    } else {
        oss << std::fixed << std::setprecision(2) << value;
    }
    return oss.str();
}

void GraphWidget::DrawAxisLabels() {
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA, 10);
    
    // X-axis labels
    int numXLabels = 5;
    for (int i = 0; i <= numXLabels; ++i) {
        double worldX = coordParams_.xMin + (coordParams_.xMax - coordParams_.xMin) * i / numXLabels;
        int screenX, screenY;
        WorldToScreen(worldX, 0, screenX, screenY);
        
        std::string label = FormatNumber(worldX);
        fl_draw(label.c_str(), screenX - 20, y() + h() - 5);
    }
    
    // Y-axis labels
    int numYLabels = 5;
    for (int i = 0; i <= numYLabels; ++i) {
        double worldY = coordParams_.yMin + (coordParams_.yMax - coordParams_.yMin) * i / numYLabels;
        int screenX, screenY;
        WorldToScreen(0, worldY, screenX, screenY);
        
        std::string label = FormatNumber(worldY);
        fl_draw(label.c_str(), x() + 5, screenY + 5);
    }
}

void GraphWidget::DrawGrid() {
    fl_color(220, 220, 220);
    
    int numGridLines = 10;
    
    // Vertical grid lines
    for (int i = 0; i <= numGridLines; ++i) {
        double worldX = coordParams_.xMin + (coordParams_.xMax - coordParams_.xMin) * i / numGridLines;
        int screenX1, screenY1, screenX2, screenY2;
        WorldToScreen(worldX, coordParams_.yMin, screenX1, screenY1);
        WorldToScreen(worldX, coordParams_.yMax, screenX2, screenY2);
        fl_line(screenX1, screenY1, screenX2, screenY2);
    }
    
    // Horizontal grid lines
    for (int i = 0; i <= numGridLines; ++i) {
        double worldY = coordParams_.yMin + (coordParams_.yMax - coordParams_.yMin) * i / numGridLines;
        int screenX1, screenY1, screenX2, screenY2;
        WorldToScreen(coordParams_.xMin, worldY, screenX1, screenY1);
        WorldToScreen(coordParams_.xMax, worldY, screenX2, screenY2);
        fl_line(screenX1, screenY1, screenX2, screenY2);
    }
}

void GraphWidget::DrawAxes() {
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 2);
    
    // X-axis
    int screenX1, screenY1, screenX2, screenY2;
    WorldToScreen(coordParams_.xMin, 0, screenX1, screenY1);
    WorldToScreen(coordParams_.xMax, 0, screenX2, screenY2);
    fl_line(screenX1, screenY1, screenX2, screenY2);
    
    // Y-axis
    WorldToScreen(0, coordParams_.yMin, screenX1, screenY1);
    WorldToScreen(0, coordParams_.yMax, screenX2, screenY2);
    fl_line(screenX1, screenY1, screenX2, screenY2);
    
    fl_line_style(0);
}

void GraphWidget::DrawCoordinateSystem() {
    DrawGrid();
    DrawAxes();
    DrawAxisLabels();
}

void GraphWidget::draw() {
    // Draw background
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    
    if (curves_.empty()) {
        return;
    }
    
    DrawCoordinateSystem();
    
    // Draw all curves
    int colorIndex = 0;
    for (const auto& curve : curves_) {
        if (curve) {
            curve->Draw(this, coordParams_);
            colorIndex++;
        }
    }
}

void LoadedParametricCurve2D::Draw(GraphWidget* widget, const CoordSystemParams& params) {
    if (xVals_.empty()) return;
    
    auto color = GraphWidget::GetColor(index_);
    fl_color(color.r, color.g, color.b);
    fl_line_style(FL_SOLID, 2);
    
    // Draw the parametric curve as connected line segments
    for (size_t i = 0; i < xVals_.size() - 1; ++i) {
        int x1, y1, x2, y2;
        widget->WorldToScreen(xVals_[i], yVals_[i], x1, y1);
        widget->WorldToScreen(xVals_[i + 1], yVals_[i + 1], x2, y2);
        fl_line(x1, y1, x2, y2);
    }
    
    fl_line_style(0);
}
