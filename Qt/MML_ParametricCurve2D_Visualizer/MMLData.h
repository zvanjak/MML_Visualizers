#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>

// Color structure for curve colors
struct Color {
    float r, g, b;
    Color(float red = 0.0f, float green = 0.0f, float blue = 0.0f) 
        : r(red), g(green), b(blue) {}
};

// Standard color palette matching WPF/FLTK
inline const std::vector<Color>& GetColorPalette() {
    static const std::vector<Color> palette = {
        Color(0.0f, 0.0f, 0.0f),       // Black
        Color(0.0f, 0.0f, 1.0f),       // Blue
        Color(1.0f, 0.0f, 0.0f),       // Red
        Color(0.0f, 0.5f, 0.0f),       // Green
        Color(1.0f, 0.65f, 0.0f),      // Orange
        Color(0.5f, 0.0f, 0.5f),       // Purple
        Color(0.55f, 0.27f, 0.07f),    // Brown
        Color(0.0f, 1.0f, 1.0f),       // Cyan
        Color(1.0f, 0.0f, 1.0f),       // Magenta
        Color(0.5f, 0.5f, 0.5f),       // Gray
        Color(1.0f, 1.0f, 0.0f)        // Yellow
    };
    return palette;
}

inline Color GetColorByIndex(int index) {
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

// Class for loaded parametric curve 2D
class LoadedParamCurve2D {
private:
    std::string title_;
    int index_;
    std::vector<double> tVals_;
    std::vector<double> xVals_;
    std::vector<double> yVals_;
    CurveDrawStyle style_;
    Color color_;

public:
    LoadedParamCurve2D(const std::string& title, int index) 
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
    
    Color GetColor() const { return color_; }
    void SetColor(const Color& color) { color_ = color; }
    
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
    
    size_t GetNumPoints() const { return xVals_.size(); }
    
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
};

#endif // MML_DATA_H
