#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <limits>
#include "AxisTickCalculator.h"

// Color definition
struct CurveColor {
    unsigned char r, g, b;
    CurveColor(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0)
        : r(red), g(green), b(blue) {}
};

// Standard color palette
inline const std::vector<CurveColor>& GetColorPalette() {
    static const std::vector<CurveColor> palette = {
        CurveColor(0, 0, 0),       // Black
        CurveColor(0, 0, 255),     // Blue
        CurveColor(255, 0, 0),     // Red
        CurveColor(0, 128, 0),     // Green
        CurveColor(255, 165, 0),   // Orange
        CurveColor(128, 0, 128),   // Purple
        CurveColor(139, 69, 19),   // Brown
        CurveColor(0, 255, 255),   // Cyan
        CurveColor(255, 0, 255),   // Magenta
        CurveColor(128, 128, 128), // Gray
        CurveColor(255, 255, 0)    // Yellow
    };
    return palette;
}

inline CurveColor GetColorByIndex(int index) {
    const auto& palette = GetColorPalette();
    return palette[index % palette.size()];
}

// Draw style for curves
struct CurveDrawStyle {
    double lineWidth = 2.0;       // Thicker for parametric curves
    bool showPoints = false;
    double pointRadius = 3.0;
    bool visible = true;
};

// Structure to hold coordinate system parameters
struct CoordSystemParams {
    double xMin, xMax;
    double yMin, yMax;
    double tMin, tMax;
    int numPoints;
    
    // Display area (within widget)
    int drawX, drawY, drawWidth, drawHeight;
    
    // Margins for axis labels
    static constexpr int MARGIN_LEFT = 60;
    static constexpr int MARGIN_RIGHT = 20;
    static constexpr int MARGIN_TOP = 20;
    static constexpr int MARGIN_BOTTOM = 40;
    
    // Scaling
    double scaleX, scaleY;
    
    // Tick info
    AxisTickInfo xTickInfo;
    AxisTickInfo yTickInfo;
    
    // Settings
    bool preserveAspectRatio = false;
    bool showGrid = true;
    bool showAxisLabels = true;
    
    CoordSystemParams() : 
        xMin(-1), xMax(1), yMin(-1), yMax(1),
        tMin(0), tMax(1), numPoints(0),
        drawX(0), drawY(0), drawWidth(800), drawHeight(600),
        scaleX(1), scaleY(1) {}
    
    void UpdateFromWidget(int widgetW, int widgetH) {
        drawX = MARGIN_LEFT;
        drawY = MARGIN_TOP;
        drawWidth = widgetW - MARGIN_LEFT - MARGIN_RIGHT;
        drawHeight = widgetH - MARGIN_TOP - MARGIN_BOTTOM;
        
        if (drawWidth < 100) drawWidth = 100;
        if (drawHeight < 100) drawHeight = 100;
        
        UpdateScaling();
    }
    
    void UpdateScaling() {
        double xRange = xTickInfo.max - xTickInfo.min;
        double yRange = yTickInfo.max - yTickInfo.min;
        
        if (xRange <= 0) xRange = 1;
        if (yRange <= 0) yRange = 1;
        
        scaleX = drawWidth / xRange;
        scaleY = drawHeight / yRange;
        
        if (preserveAspectRatio) {
            double uniformScale = std::min(scaleX, scaleY);
            scaleX = uniformScale;
            scaleY = uniformScale;
        }
    }
    
    // Convert world coordinates to screen coordinates
    void WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
        screenX = drawX + static_cast<int>((worldX - xTickInfo.min) * scaleX);
        screenY = drawY + drawHeight - static_cast<int>((worldY - yTickInfo.min) * scaleY);
    }
    
    // Convert screen coordinates to world coordinates
    void ScreenToWorld(int screenX, int screenY, double& worldX, double& worldY) const {
        worldX = xTickInfo.min + (screenX - drawX) / scaleX;
        worldY = yTickInfo.max - (screenY - drawY) / scaleY;
    }
};

// Forward declaration
class GraphWidget;

// Class for loaded parametric curve 2D
class LoadedParametricCurve2D {
private:
    std::string title_;
    int index_;
    std::vector<double> tVals_;
    std::vector<double> xVals_;
    std::vector<double> yVals_;
    CurveDrawStyle style_;
    CurveColor color_;

public:
    LoadedParametricCurve2D(const std::string& title, int index) 
        : title_(title), index_(index), color_(GetColorByIndex(index)) {}
    
    void AddPoint(double t, double x, double y) {
        tVals_.push_back(t);
        xVals_.push_back(x);
        yVals_.push_back(y);
    }
    
    // Getters
    const std::vector<double>& GetTVals() const { return tVals_; }
    const std::vector<double>& GetXVals() const { return xVals_; }
    const std::vector<double>& GetYVals() const { return yVals_; }
    int GetIndex() const { return index_; }
    const std::string& GetTitle() const { return title_; }
    void SetTitle(const std::string& title) { title_ = title; }
    
    CurveDrawStyle& GetStyle() { return style_; }
    const CurveDrawStyle& GetStyle() const { return style_; }
    
    CurveColor GetColor() const { return color_; }
    void SetColor(const CurveColor& color) { color_ = color; }
    
    bool IsVisible() const { return style_.visible; }
    void SetVisible(bool visible) { style_.visible = visible; }
    
    // Get point at specific index (for animation)
    bool GetPointAt(size_t index, double& x, double& y) const {
        if (index < xVals_.size()) {
            x = xVals_[index];
            y = yVals_[index];
            return true;
        }
        return false;
    }
    
    double GetMinX() const {
        if (xVals_.empty()) return 0;
        return *std::min_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMaxX() const {
        if (xVals_.empty()) return 1;
        return *std::max_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMinY() const {
        if (yVals_.empty()) return 0;
        return *std::min_element(yVals_.begin(), yVals_.end());
    }
    
    double GetMaxY() const {
        if (yVals_.empty()) return 1;
        return *std::max_element(yVals_.begin(), yVals_.end());
    }
    
    double GetMinT() const {
        if (tVals_.empty()) return 0;
        return *std::min_element(tVals_.begin(), tVals_.end());
    }
    
    double GetMaxT() const {
        if (tVals_.empty()) return 1;
        return *std::max_element(tVals_.begin(), tVals_.end());
    }
    
    size_t GetNumPoints() const {
        return xVals_.size();
    }
    
    // Draw the curve
    void Draw(const CoordSystemParams& params);
    
    // Draw animation marker at specific point index
    void DrawMarker(const CoordSystemParams& params, size_t pointIndex);
};

#endif // MML_DATA_H
