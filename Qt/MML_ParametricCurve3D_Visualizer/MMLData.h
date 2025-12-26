#ifndef MML_DATA_H
#define MML_DATA_H

#define NOMINMAX
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <functional>

// Structure to represent a 3D point
struct Point3D {
    double x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

// Structure to represent RGB color (0-1 range for OpenGL)
struct Color {
    float r, g, b;
    
    Color() : r(0), g(0), b(0) {}
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
    
    // Constructor from 0-255 values
    static Color FromRGB(int r, int g, int b) {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f);
    }
};

// 11-color palette matching WPF specification
inline std::vector<Color> GetColorPalette() {
    return {
        Color(0.0f, 0.0f, 0.0f),       // 0: Black
        Color(0.0f, 0.0f, 1.0f),       // 1: Blue
        Color(1.0f, 0.0f, 0.0f),       // 2: Red
        Color(0.0f, 0.5f, 0.0f),       // 3: Green
        Color(1.0f, 0.65f, 0.0f),      // 4: Orange
        Color(0.5f, 0.0f, 0.5f),       // 5: Purple
        Color(0.65f, 0.16f, 0.16f),    // 6: Brown
        Color(0.0f, 1.0f, 1.0f),       // 7: Cyan
        Color(1.0f, 0.0f, 1.0f),       // 8: Magenta
        Color(0.5f, 0.5f, 0.5f),       // 9: Gray
        Color(1.0f, 1.0f, 0.0f),       // 10: Yellow
    };
}

inline Color GetColorByIndex(int index) {
    static auto palette = GetColorPalette();
    return palette[index % palette.size()];
}

// Class representing a 3D parametric curve
class LoadedParametricCurve3D {
public:
    LoadedParametricCurve3D(const std::string& name, double t1, double t2)
        : name_(name), t1_(t1), t2_(t2), visible_(true), color_(0, 0, 0) {}
    
    void AddPoint(double t, double x, double y, double z) {
        tVals_.push_back(t);
        xVals_.push_back(x);
        yVals_.push_back(y);
        zVals_.push_back(z);
    }
    
    const std::string& GetName() const { return name_; }
    double GetT1() const { return t1_; }
    double GetT2() const { return t2_; }
    size_t GetNumPoints() const { return tVals_.size(); }
    
    const std::vector<double>& GetTVals() const { return tVals_; }
    const std::vector<double>& GetXVals() const { return xVals_; }
    const std::vector<double>& GetYVals() const { return yVals_; }
    const std::vector<double>& GetZVals() const { return zVals_; }
    
    // Visibility
    bool IsVisible() const { return visible_; }
    void SetVisible(bool visible) { visible_ = visible; }
    
    // Color
    Color GetColor() const { return color_; }
    void SetColor(const Color& color) { color_ = color; }
    
    // Get point at index
    Point3D GetPoint(size_t i) const {
        if (i >= tVals_.size()) {
            throw std::out_of_range("Point index out of range");
        }
        return Point3D(xVals_[i], yVals_[i], zVals_[i]);
    }
    
    // Get T value at index
    double GetT(size_t i) const {
        if (i >= tVals_.size()) {
            return t1_;
        }
        return tVals_[i];
    }
    
    // Get bounding box
    void GetBounds(double& xMin, double& xMax, 
                   double& yMin, double& yMax,
                   double& zMin, double& zMax) const {
        if (xVals_.empty()) {
            xMin = yMin = zMin = -1.0;
            xMax = yMax = zMax = 1.0;
            return;
        }
        
        xMin = *std::min_element(xVals_.begin(), xVals_.end());
        xMax = *std::max_element(xVals_.begin(), xVals_.end());
        yMin = *std::min_element(yVals_.begin(), yVals_.end());
        yMax = *std::max_element(yVals_.begin(), yVals_.end());
        zMin = *std::min_element(zVals_.begin(), zVals_.end());
        zMax = *std::max_element(zVals_.begin(), zVals_.end());
    }
    
private:
    std::string name_;
    double t1_, t2_;
    std::vector<double> tVals_;
    std::vector<double> xVals_;
    std::vector<double> yVals_;
    std::vector<double> zVals_;
    bool visible_;
    Color color_;
};

// Animation callback type
using AnimationCallback = std::function<void()>;

#endif // MML_DATA_H
