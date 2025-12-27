#define NOMINMAX
#define _USE_MATH_DEFINES
#include "VectorFieldWidget.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

VectorFieldWidget::VectorFieldWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L)
    , magnitudeScale_(1.0)
    , optimalScale_(1.0)
    , arrowSize_(8)
    , colorByMagnitude_(true)
    , normalizeVectors_(false)
    , preserveAspectRatio_(false)
    , arrowColorIndex_(0)  // Black
{
}

void VectorFieldWidget::SetVectorField(std::unique_ptr<VectorField2D> field) {
    vectorField_ = std::move(field);
    if (vectorField_) {
        optimalScale_ = vectorField_->CalculateOptimalScale();
    }
    InitializeCoordParams();
}

void VectorFieldWidget::ClearVectorField() {
    vectorField_.reset();
    optimalScale_ = 1.0;
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
    
    if (preserveAspectRatio_) {
        // Use uniform scale (minimum of X/Y)
        double scaleX = (coordParams_.windowWidth * 0.9) / xRange;
        double scaleY = (coordParams_.windowHeight * 0.9) / yRange;
        double uniformScale = std::min(scaleX, scaleY);
        coordParams_.scaleX = uniformScale;
        coordParams_.scaleY = uniformScale;
    } else {
        // Stretch to fill
        coordParams_.scaleX = (coordParams_.windowWidth * 0.9) / xRange;
        coordParams_.scaleY = (coordParams_.windowHeight * 0.9) / yRange;
    }
    
    double midPointX = (coordParams_.xMin + coordParams_.xMax) / 2;
    double midPointY = (coordParams_.yMin + coordParams_.yMax) / 2;
    
    coordParams_.centerX = coordParams_.windowWidth / 2 - midPointX * coordParams_.scaleX;
    coordParams_.centerY = coordParams_.windowHeight / 2 + midPointY * coordParams_.scaleY;
}

void VectorFieldWidget::WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
    screenX = static_cast<int>(coordParams_.centerX + worldX * coordParams_.scaleX);
    screenY = static_cast<int>(coordParams_.centerY - worldY * coordParams_.scaleY);
    
    // Add widget position offset
    screenX += x();
    screenY += y();
}

Fl_Color VectorFieldWidget::GetMagnitudeColor(double magnitude) const {
    if (!vectorField_) return FL_BLACK;
    
    double minMag = vectorField_->GetMinVectorMagnitude();
    double maxMag = vectorField_->GetMaxVectorMagnitude();
    
    // Normalize magnitude to [0, 1]
    double t = 0.5;
    if (maxMag > minMag) {
        t = (magnitude - minMag) / (maxMag - minMag);
    }
    t = std::max(0.0, std::min(1.0, t));  // Clamp to [0, 1]
    
    // Color gradient: Blue(0,0,255) -> Cyan(0,255,255) -> Green(0,255,0) -> Yellow(255,255,0) -> Red(255,0,0)
    // WPF spec: 5 color stops at positions 0, 0.25, 0.5, 0.75, 1.0
    int r, g, b;
    
    if (t < 0.25) {
        // Blue to Cyan: (0,0,255) -> (0,255,255)
        double localT = t / 0.25;
        r = 0;
        g = static_cast<int>(255 * localT);
        b = 255;
    } else if (t < 0.5) {
        // Cyan to Green: (0,255,255) -> (0,255,0)
        double localT = (t - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = static_cast<int>(255 * (1.0 - localT));
    } else if (t < 0.75) {
        // Green to Yellow: (0,255,0) -> (255,255,0)
        double localT = (t - 0.5) / 0.25;
        r = static_cast<int>(255 * localT);
        g = 255;
        b = 0;
    } else {
        // Yellow to Red: (255,255,0) -> (255,0,0)
        double localT = (t - 0.75) / 0.25;
        r = 255;
        g = static_cast<int>(255 * (1.0 - localT));
        b = 0;
    }
    
    return fl_rgb_color(r, g, b);
}

Fl_Color VectorFieldWidget::GetArrowColor() const {
    // Solid colors by index: Black, Blue, Red, Green, Orange, Purple
    switch (arrowColorIndex_) {
        case 0: return FL_BLACK;
        case 1: return FL_BLUE;
        case 2: return FL_RED;
        case 3: return fl_rgb_color(0, 128, 0);  // Green
        case 4: return fl_rgb_color(255, 165, 0);  // Orange
        case 5: return fl_rgb_color(128, 0, 128);  // Purple
        default: return FL_BLACK;
    }
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

void VectorFieldWidget::DrawArrowHead(int x1, int y1, int x2, int y2, Fl_Color color) {
    // Calculate arrow direction
    double dx = x2 - x1;
    double dy = y2 - y1;
    double length = std::sqrt(dx * dx + dy * dy);
    
    if (length < 1) return;  // Vector too short
    
    double angle = std::atan2(dy, dx);
    double arrowAngle = 25.0 * M_PI / 180.0;
    
    int xArrow1 = x2 - static_cast<int>(arrowSize_ * std::cos(angle + arrowAngle));
    int yArrow1 = y2 - static_cast<int>(arrowSize_ * std::sin(angle + arrowAngle));
    int xArrow2 = x2 - static_cast<int>(arrowSize_ * std::cos(angle - arrowAngle));
    int yArrow2 = y2 - static_cast<int>(arrowSize_ * std::sin(angle - arrowAngle));
    
    fl_color(color);
    fl_line(x2, y2, xArrow1, yArrow1);
    fl_line(x2, y2, xArrow2, yArrow2);
}

void VectorFieldWidget::DrawVector(const VectorRepr& vec) {
    double vx = vec.vector.x;
    double vy = vec.vector.y;
    double magnitude = vec.vector.Magnitude();
    
    if (magnitude < 1e-10) return;  // Skip zero vectors
    
    // Normalize if requested
    if (normalizeVectors_) {
        vx /= magnitude;
        vy /= magnitude;
    }
    
    // Calculate effective scale
    double effectiveScale = optimalScale_ * magnitudeScale_;
    
    // Calculate half-vector for centering (WPF spec: arrows centered at data points)
    double halfVx = effectiveScale * vx / 2.0;
    double halfVy = effectiveScale * vy / 2.0;
    
    // Start and end in world coordinates (centered on position)
    double startX = vec.position.x - halfVx;
    double startY = vec.position.y - halfVy;
    double endX = vec.position.x + halfVx;
    double endY = vec.position.y + halfVy;
    
    // Convert to screen coordinates
    int x1, y1, x2, y2;
    WorldToScreen(startX, startY, x1, y1);
    WorldToScreen(endX, endY, x2, y2);
    
    // Determine color
    Fl_Color color;
    if (colorByMagnitude_) {
        color = GetMagnitudeColor(magnitude);
    } else {
        color = GetArrowColor();
    }
    
    // Draw vector line
    fl_color(color);
    fl_line_style(FL_SOLID, 2);
    fl_line(x1, y1, x2, y2);
    
    // Draw arrow head
    DrawArrowHead(x1, y1, x2, y2, color);
    
    fl_line_style(0);
}

void VectorFieldWidget::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    InitializeCoordParams();
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
