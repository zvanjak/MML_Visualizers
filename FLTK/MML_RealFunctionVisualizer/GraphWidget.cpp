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
    : Fl_Widget(X, Y, W, H, L) {
}

void GraphWidget::AddFunction(std::unique_ptr<LoadedFunction> func) {
    functions_.push_back(std::move(func));
}

void GraphWidget::ClearFunctions() {
    functions_.clear();
}

void GraphWidget::InitializeCoordParams() {
    if (functions_.empty()) {
        coordParams_.xMin = -10;
        coordParams_.xMax = 10;
        coordParams_.yMin = -10;
        coordParams_.yMax = 10;
        coordParams_.numPoints = 0;
    } else {
        coordParams_.xMin = functions_[0]->GetMinX();
        coordParams_.xMax = functions_[0]->GetMaxX();
        coordParams_.yMin = functions_[0]->GetMinY();
        coordParams_.yMax = functions_[0]->GetMaxY();
        coordParams_.numPoints = functions_[0]->GetNumPoints();
        
        for (size_t i = 1; i < functions_.size(); ++i) {
            coordParams_.xMin = std::min(coordParams_.xMin, functions_[i]->GetMinX());
            coordParams_.xMax = std::max(coordParams_.xMax, functions_[i]->GetMaxX());
            coordParams_.yMin = std::min(coordParams_.yMin, functions_[i]->GetMinY());
            coordParams_.yMax = std::max(coordParams_.yMax, functions_[i]->GetMaxY());
        }
    }
    
    coordParams_.windowWidth = w();
    coordParams_.windowHeight = h();
    
    double midPoint = (coordParams_.xMin + coordParams_.xMax) / 2.0;
    double midPointY = (coordParams_.yMin + coordParams_.yMax) / 2.0;
    
    coordParams_.scaleX = coordParams_.windowWidth / (coordParams_.xMax - coordParams_.xMin) * 0.9;
    coordParams_.scaleY = coordParams_.windowHeight / (coordParams_.yMax - coordParams_.yMin) * 0.9;
    coordParams_.centerX = coordParams_.windowWidth / 2.0 - midPoint * coordParams_.scaleX;
    coordParams_.centerY = coordParams_.windowHeight / 2.0 + midPointY * coordParams_.scaleY;
}

void GraphWidget::WorldToScreen(double x, double y, int& screenX, int& screenY) const {
    screenX = static_cast<int>(this->x() + coordParams_.centerX + x * coordParams_.scaleX);
    screenY = static_cast<int>(this->y() + coordParams_.centerY - y * coordParams_.scaleY);
}

std::string GraphWidget::FormatNumber(double value) const {
    std::ostringstream oss;
    if (std::abs(value) < 0.001 && value != 0) {
        oss << std::scientific << std::setprecision(2) << value;
    } else if (std::abs(value) > 1000) {
        oss << std::scientific << std::setprecision(2) << value;
    } else {
        oss << std::fixed << std::setprecision(2) << value;
    }
    return oss.str();
}

void GraphWidget::DrawCoordinateSystem() {
    DrawGrid();
    DrawAxes();
    DrawAxisLabels();
}

void GraphWidget::DrawGrid() {
    fl_color(220, 220, 220); // Light gray for grid
    
    // Vertical grid lines
    int numVerticalLines = 10;
    for (int i = 0; i <= numVerticalLines; ++i) {
        double worldX = coordParams_.xMin + i * (coordParams_.xMax - coordParams_.xMin) / numVerticalLines;
        int screenX, screenY1, screenY2;
        WorldToScreen(worldX, coordParams_.yMin, screenX, screenY1);
        WorldToScreen(worldX, coordParams_.yMax, screenX, screenY2);
        fl_line(screenX, screenY1, screenX, screenY2);
    }
    
    // Horizontal grid lines
    int numHorizontalLines = 10;
    for (int i = 0; i <= numHorizontalLines; ++i) {
        double worldY = coordParams_.yMin + i * (coordParams_.yMax - coordParams_.yMin) / numHorizontalLines;
        int screenX1, screenX2, screenY;
        WorldToScreen(coordParams_.xMin, worldY, screenX1, screenY);
        WorldToScreen(coordParams_.xMax, worldY, screenX2, screenY);
        fl_line(screenX1, screenY, screenX2, screenY);
    }
}

void GraphWidget::DrawAxes() {
    fl_color(0, 0, 0); // Black for axes
    fl_line_style(FL_SOLID, 2);
    
    // X-axis (y = 0)
    if (coordParams_.yMin <= 0 && coordParams_.yMax >= 0) {
        int screenX1, screenX2, screenY;
        WorldToScreen(coordParams_.xMin, 0, screenX1, screenY);
        WorldToScreen(coordParams_.xMax, 0, screenX2, screenY);
        fl_line(screenX1, screenY, screenX2, screenY);
    }
    
    // Y-axis (x = 0)
    if (coordParams_.xMin <= 0 && coordParams_.xMax >= 0) {
        int screenX, screenY1, screenY2;
        WorldToScreen(0, coordParams_.yMin, screenX, screenY1);
        WorldToScreen(0, coordParams_.yMax, screenX, screenY2);
        fl_line(screenX, screenY1, screenX, screenY2);
    }
    
    fl_line_style(FL_SOLID, 1);
}

void GraphWidget::DrawAxisLabels() {
    fl_color(0, 0, 0);
    fl_font(FL_HELVETICA, 10);
    
    // X-axis labels
    int numXLabels = 5;
    for (int i = 0; i <= numXLabels; ++i) {
        double worldX = coordParams_.xMin + i * (coordParams_.xMax - coordParams_.xMin) / numXLabels;
        int screenX, screenY;
        WorldToScreen(worldX, 0, screenX, screenY);
        
        std::string label = FormatNumber(worldX);
        fl_draw(label.c_str(), screenX - 20, screenY + 15);
    }
    
    // Y-axis labels
    int numYLabels = 5;
    for (int i = 0; i <= numYLabels; ++i) {
        double worldY = coordParams_.yMin + i * (coordParams_.yMax - coordParams_.yMin) / numYLabels;
        int screenX, screenY;
        WorldToScreen(0, worldY, screenX, screenY);
        
        std::string label = FormatNumber(worldY);
        fl_draw(label.c_str(), screenX - 50, screenY + 5);
    }
}

void GraphWidget::draw() {
    // Clear background
    fl_color(255, 255, 255); // White background
    fl_rectf(x(), y(), w(), h());
    
    InitializeCoordParams();
    DrawCoordinateSystem();
    
    // Draw all functions
    for (auto& func : functions_) {
        func->Draw(this, coordParams_);
    }
    
    // Draw border
    fl_color(0, 0, 0);
    fl_rect(x(), y(), w(), h());
}

// Implementation of Draw methods for loaded functions

void SingleLoadedFunction::Draw(GraphWidget* widget, const CoordSystemParams& params) {
    const auto& xVals = GetXVals();
    const auto& yVals = GetYVals();
    
    if (xVals.size() < 2) return;
    
    Color color = GraphWidget::GetColor(GetIndex());
    fl_color(color.r, color.g, color.b);
    fl_line_style(FL_SOLID, 2);
    
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
    
    fl_line_style(FL_SOLID, 2);
    
    for (size_t funcIndex = 0; funcIndex < yVals.size(); ++funcIndex) {
        Color color = GraphWidget::GetColor(funcIndex);
        fl_color(color.r, color.g, color.b);
        
        for (size_t i = 0; i < xVals.size() - 1; ++i) {
            int x1, y1, x2, y2;
            widget->WorldToScreen(xVals[i], yVals[funcIndex][i], x1, y1);
            widget->WorldToScreen(xVals[i + 1], yVals[funcIndex][i + 1], x2, y2);
            fl_line(x1, y1, x2, y2);
        }
    }
    
    fl_line_style(FL_SOLID, 1);
}
