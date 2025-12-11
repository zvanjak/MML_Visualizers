#ifndef MML_DATA_H
#define MML_DATA_H

#define NOMINMAX
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

struct Point2D {
    double x;
    double y;
    
    Point2D() : x(0), y(0) {}
    Point2D(double x_, double y_) : x(x_), y(y_) {}
};

struct Color {
    float r, g, b;
    
    Color() : r(1.0f), g(1.0f), b(1.0f) {}
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
};

class LoadedRealFunction {
public:
    LoadedRealFunction(const std::string& title, const Color& color)
        : title_(title), color_(color), xMin_(0), xMax_(1) {}
    
    void AddPoint(double x, double y) {
        points_.push_back(Point2D(x, y));
    }
    
    void SetXRange(double xMin, double xMax) {
        xMin_ = xMin;
        xMax_ = xMax;
    }
    
    const std::vector<Point2D>& GetPoints() const { return points_; }
    const std::string& GetTitle() const { return title_; }
    const Color& GetColor() const { return color_; }
    
    int GetNumPoints() const { return static_cast<int>(points_.size()); }
    
    double GetMinX() const {
        if (points_.empty()) return 0.0;
        double minX = std::numeric_limits<double>::max();
        for (const auto& p : points_) {
            minX = std::min(minX, p.x);
        }
        return minX;
    }
    
    double GetMaxX() const {
        if (points_.empty()) return 1.0;
        double maxX = std::numeric_limits<double>::lowest();
        for (const auto& p : points_) {
            maxX = std::max(maxX, p.x);
        }
        return maxX;
    }
    
    double GetMinY() const {
        if (points_.empty()) return 0.0;
        double minY = std::numeric_limits<double>::max();
        for (const auto& p : points_) {
            minY = std::min(minY, p.y);
        }
        return minY;
    }
    
    double GetMaxY() const {
        if (points_.empty()) return 1.0;
        double maxY = std::numeric_limits<double>::lowest();
        for (const auto& p : points_) {
            maxY = std::max(maxY, p.y);
        }
        return maxY;
    }

private:
    std::string title_;
    Color color_;
    std::vector<Point2D> points_;
    double xMin_;
    double xMax_;
};

#endif // MML_DATA_H
