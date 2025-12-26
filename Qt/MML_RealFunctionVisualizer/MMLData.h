#ifndef MML_DATA_H
#define MML_DATA_H

#define NOMINMAX
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <memory>

struct Point2D {
    double x;
    double y;
    
    Point2D() : x(0), y(0) {}
    Point2D(double x_, double y_) : x(x_), y(y_) {}
};

struct Color {
    float r, g, b;
    
    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
};

// Base class for loaded functions
class LoadedFunction {
public:
    virtual ~LoadedFunction() = default;
    
    virtual const std::string& GetTitle() const = 0;
    virtual int GetNumPoints() const = 0;
    virtual int GetDimension() const = 0;
    
    virtual double GetMinX() const = 0;
    virtual double GetMaxX() const = 0;
    virtual double GetMinY() const = 0;
    virtual double GetMaxY() const = 0;
    
    // Visibility control
    virtual bool IsVisible() const { return visible_; }
    virtual void SetVisible(bool visible) { visible_ = visible; }
    
    // For multi-function: get/set sub-function visibility
    virtual bool IsFunctionVisible(int index) const { return visible_; }
    virtual void SetFunctionVisible(int index, bool visible) { visible_ = visible; }
    
    // Get function title for sub-functions (for multi-function)
    virtual std::string GetFunctionTitle(int index) const { return GetTitle(); }
    
    // Get color for a sub-function (for multi-function)
    virtual Color GetFunctionColor(int index) const = 0;
    
protected:
    bool visible_ = true;
};

// Single real function (y = f(x))
class LoadedRealFunction : public LoadedFunction {
public:
    LoadedRealFunction(const std::string& title, const Color& color)
        : title_(title), color_(color), xMin_(0), xMax_(1) {}
    
    // Alternative constructor that takes an index (for parser compatibility)
    LoadedRealFunction(const std::string& title, int /*index*/)
        : title_(title), color_(0.0f, 0.0f, 0.0f), xMin_(0), xMax_(1) {}
    
    void AddPoint(double x, double y) {
        points_.push_back(Point2D(x, y));
    }
    
    void SetXRange(double xMin, double xMax) {
        xMin_ = xMin;
        xMax_ = xMax;
    }
    
    const std::vector<Point2D>& GetPoints() const { return points_; }
    const std::string& GetTitle() const override { return title_; }
    const Color& GetColor() const { return color_; }
    void SetColor(const Color& color) { color_ = color; }
    
    int GetNumPoints() const override { return static_cast<int>(points_.size()); }
    int GetDimension() const override { return 1; }
    
    Color GetFunctionColor(int /*index*/) const override { return color_; }
    
    double GetMinX() const override {
        if (points_.empty()) return 0.0;
        double minX = std::numeric_limits<double>::max();
        for (const auto& p : points_) {
            minX = std::min(minX, p.x);
        }
        return minX;
    }
    
    double GetMaxX() const override {
        if (points_.empty()) return 1.0;
        double maxX = std::numeric_limits<double>::lowest();
        for (const auto& p : points_) {
            maxX = std::max(maxX, p.x);
        }
        return maxX;
    }
    
    double GetMinY() const override {
        if (points_.empty()) return 0.0;
        double minY = std::numeric_limits<double>::max();
        for (const auto& p : points_) {
            minY = std::min(minY, p.y);
        }
        return minY;
    }
    
    double GetMaxY() const override {
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

// Multiple functions sharing the same x-coordinates
class MultiLoadedFunction : public LoadedFunction {
public:
    MultiLoadedFunction(const std::string& title, const std::vector<std::string>& legend)
        : title_(title), legend_(legend) {
        // Initialize visibility for all sub-functions
        functionVisibility_.resize(legend.size(), true);
    }
    
    void AddPoint(double x, const std::vector<double>& yValues) {
        xValues_.push_back(x);
        
        // Ensure we have enough vectors
        while (yValues_.size() < yValues.size()) {
            yValues_.push_back(std::vector<double>());
        }
        
        for (size_t i = 0; i < yValues.size(); ++i) {
            yValues_[i].push_back(yValues[i]);
        }
    }
    
    const std::vector<double>& GetXValues() const { return xValues_; }
    const std::vector<std::vector<double>>& GetYValues() const { return yValues_; }
    const std::vector<std::string>& GetLegend() const { return legend_; }
    
    const std::string& GetTitle() const override { return title_; }
    int GetNumPoints() const override { return static_cast<int>(xValues_.size()); }
    int GetDimension() const override { return static_cast<int>(legend_.size()); }
    
    std::string GetFunctionTitle(int index) const override {
        if (index >= 0 && index < static_cast<int>(legend_.size())) {
            return legend_[index];
        }
        return title_;
    }
    
    Color GetFunctionColor(int index) const override {
        if (index >= 0 && index < static_cast<int>(colors_.size())) {
            return colors_[index];
        }
        return Color(0, 0, 0);
    }
    
    void SetFunctionColor(int index, const Color& color) {
        while (static_cast<int>(colors_.size()) <= index) {
            colors_.push_back(Color(0, 0, 0));
        }
        colors_[index] = color;
    }
    
    bool IsFunctionVisible(int index) const override {
        if (index >= 0 && index < static_cast<int>(functionVisibility_.size())) {
            return functionVisibility_[index];
        }
        return true;
    }
    
    void SetFunctionVisible(int index, bool visible) override {
        if (index >= 0 && index < static_cast<int>(functionVisibility_.size())) {
            functionVisibility_[index] = visible;
        }
    }
    
    double GetMinX() const override {
        if (xValues_.empty()) return 0.0;
        return *std::min_element(xValues_.begin(), xValues_.end());
    }
    
    double GetMaxX() const override {
        if (xValues_.empty()) return 1.0;
        return *std::max_element(xValues_.begin(), xValues_.end());
    }
    
    double GetMinY() const override {
        double minY = std::numeric_limits<double>::max();
        for (size_t i = 0; i < yValues_.size(); ++i) {
            if (!IsFunctionVisible(static_cast<int>(i))) continue;
            for (double y : yValues_[i]) {
                minY = std::min(minY, y);
            }
        }
        return minY == std::numeric_limits<double>::max() ? 0.0 : minY;
    }
    
    double GetMaxY() const override {
        double maxY = std::numeric_limits<double>::lowest();
        for (size_t i = 0; i < yValues_.size(); ++i) {
            if (!IsFunctionVisible(static_cast<int>(i))) continue;
            for (double y : yValues_[i]) {
                maxY = std::max(maxY, y);
            }
        }
        return maxY == std::numeric_limits<double>::lowest() ? 1.0 : maxY;
    }

private:
    std::string title_;
    std::vector<std::string> legend_;
    std::vector<double> xValues_;
    std::vector<std::vector<double>> yValues_;
    std::vector<Color> colors_;
    std::vector<bool> functionVisibility_;
};

#endif // MML_DATA_H
