#define NOMINMAX
#define _USE_MATH_DEFINES
#include "VectorFieldWidget.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

VectorFieldWidget::VectorFieldWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L), vectorScaleFactor_(0.5) {
}

void VectorFieldWidget::SetVectorField(std::unique_ptr<VectorField2D> field) {
    vectorField_ = std::move(field);
    InitializeCoordParams();
}

void VectorFieldWidget::ClearVectorField() {
    vectorField_.reset();
    InitializeCoordParams();
}

void VectorFieldWidget::InitializeCoordParams() {
    if (!vectorField_) {
        coordParams_.xMin = -1;
        coordParams_.xMax = 1;
        coordParams_.yMin = -1;
        coordParams_.yMax = 1;
    } else {
        vectorField_->GetBounds(coordParams_.xMin, coordParams_.xMax, 
                               coordParams_.yMin, coordParams_.yMax);
        
        // Add 10% padding
        double xPadding = (coordParams_.xMax - coordParams_.xMin) * 0.1;
        double yPadding = (coordParams_.yMax - coordParams_.yMin) * 0.1;
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
    
    // Calculate vector scale factor based on grid spacing
    if (vectorField_) {
        double avgScale = (coordParams_.scaleX + coordParams_.scaleY) / 2;
        double maxMag = vectorField_->GetMaxVectorMagnitude();
        if (maxMag > 0) {
            vectorScaleFactor_ = (std::min(xRange, yRange) * 0.08) / maxMag;
        }
    }
}

void VectorFieldWidget::WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
    screenX = static_cast<int>(coordParams_.centerX + worldX * coordParams_.scaleX);
    screenY = static_cast<int>(coordParams_.centerY - worldY * coordParams_.scaleY);
    
    // Add widget position offset
    screenX += x();
    screenY += y();
}

std::string VectorFieldWidget::FormatNumber(double value) const {
    std::ostringstream oss;
    if (std::abs(value) < 0.001 || std::abs(value) > 1000) {
        oss << std::scientific << std::setprecision(2) << value;
    } else {
        oss << std::fixed << std::setprecision(2) << value;
    }
    return oss.str();
}

void VectorFieldWidget::DrawAxisLabels() {
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

void VectorFieldWidget::DrawGrid() {
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

void VectorFieldWidget::DrawAxes() {
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

void VectorFieldWidget::DrawCoordinateSystem() {
    DrawGrid();
    DrawAxes();
    DrawAxisLabels();
}

void VectorFieldWidget::DrawArrowHead(int x1, int y1, int x2, int y2) {
    // Calculate arrow direction
    double dx = x2 - x1;
    double dy = y2 - y1;
    double length = std::sqrt(dx * dx + dy * dy);
    
    if (length < 1) return;  // Vector too short
    
    double angle = std::atan2(dy, dx);
    double arrowSize = 10;
    double arrowAngle = 25.0 * M_PI / 180.0;
    
    int xArrow1 = x2 - static_cast<int>(arrowSize * std::cos(angle + arrowAngle));
    int yArrow1 = y2 - static_cast<int>(arrowSize * std::sin(angle + arrowAngle));
    int xArrow2 = x2 - static_cast<int>(arrowSize * std::cos(angle - arrowAngle));
    int yArrow2 = y2 - static_cast<int>(arrowSize * std::sin(angle - arrowAngle));
    
    fl_line(x2, y2, xArrow1, yArrow1);
    fl_line(x2, y2, xArrow2, yArrow2);
}

void VectorFieldWidget::DrawVector(const VectorRepr& vec) {
    int x1, y1, x2, y2;
    
    // Start point (position)
    WorldToScreen(vec.position.x, vec.position.y, x1, y1);
    
    // End point (position + scaled vector)
    double endX = vec.position.x + vec.vector.x * vectorScaleFactor_;
    double endY = vec.position.y + vec.vector.y * vectorScaleFactor_;
    WorldToScreen(endX, endY, x2, y2);
    
    // Draw vector line
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 1);
    fl_line(x1, y1, x2, y2);
    
    // Draw arrow head
    DrawArrowHead(x1, y1, x2, y2);
    
    fl_line_style(0);
}

void VectorFieldWidget::draw() {
    // Draw background
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    
    if (!vectorField_) {
        return;
    }
    
    DrawCoordinateSystem();
    
    // Draw all vectors
    const auto& vectors = vectorField_->GetVectors();
    for (const auto& vec : vectors) {
        DrawVector(vec);
    }
}
