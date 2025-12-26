#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>

// Forward declaration
class GraphWidget;

// Structure to hold coordinate system parameters
struct CoordSystemParams {
    double xMin, xMax;
    double yMin, yMax;
    int numPoints;
    double windowWidth, windowHeight;
    double scaleX, scaleY;
    double centerX, centerY;
    bool preserveAspectRatio;
    
    // Nice tick bounds (may differ from data bounds)
    double niceXMin, niceXMax;
    double niceYMin, niceYMax;
    
    CoordSystemParams() : 
        xMin(0), xMax(1), yMin(0), yMax(1), 
        numPoints(0), windowWidth(800), windowHeight(600),
        scaleX(1), scaleY(1), centerX(0), centerY(0),
        preserveAspectRatio(false),
        niceXMin(0), niceXMax(1), niceYMin(0), niceYMax(1) {}
};

// Draw style for a function
struct FunctionDrawStyle {
    unsigned char r, g, b;      // Color components
    double lineThickness;       // Line thickness in pixels
    int lineStyle;              // 0=solid, 1=dashed, 2=dotted
    bool showPoints;            // Whether to show data points
    double pointRadius;         // Radius of data point markers
    bool visible;               // Whether the function is visible
    
    FunctionDrawStyle() : 
        r(0), g(0), b(0), lineThickness(2.0), 
        lineStyle(0), showPoints(false), pointRadius(3.0), 
        visible(true) {}
        
    FunctionDrawStyle(unsigned char red, unsigned char green, unsigned char blue) :
        r(red), g(green), b(blue), lineThickness(2.0),
        lineStyle(0), showPoints(false), pointRadius(3.0),
        visible(true) {}
};

// Base class for loaded functions
class LoadedFunction {
protected:
    FunctionDrawStyle drawStyle_;
    
public:
    virtual ~LoadedFunction() = default;
    
    virtual void Draw(GraphWidget* widget, const CoordSystemParams& params) = 0;
    virtual double GetMinX() const = 0;
    virtual double GetMaxX() const = 0;
    virtual double GetMinY() const = 0;
    virtual double GetMaxY() const = 0;
    virtual int GetNumPoints() const = 0;
    virtual std::string GetTitle() const = 0;
    virtual int GetDimension() const { return 1; }
    virtual std::string GetFunctionTitle(int index) const { return GetTitle(); }
    
    // Visibility control
    virtual bool IsVisible() const { return drawStyle_.visible; }
    virtual void SetVisible(bool visible) { drawStyle_.visible = visible; }
    
    // Get visibility for specific sub-function (for multi-function)
    virtual bool IsFunctionVisible(int index) const { return drawStyle_.visible; }
    virtual void SetFunctionVisible(int index, bool visible) { drawStyle_.visible = visible; }
    
    // Draw style access
    FunctionDrawStyle& GetDrawStyle() { return drawStyle_; }
    const FunctionDrawStyle& GetDrawStyle() const { return drawStyle_; }
    void SetDrawStyle(const FunctionDrawStyle& style) { drawStyle_ = style; }
};

// Single function data
class SingleLoadedFunction : public LoadedFunction {
private:
    std::string title_;
    int index_;
    std::vector<double> xVals_;
    std::vector<double> yVals_;

public:
    SingleLoadedFunction(const std::string& title, int index) 
        : title_(title), index_(index) {}
    
    void AddPoint(double x, double y) {
        xVals_.push_back(x);
        yVals_.push_back(y);
    }
    
    const std::vector<double>& GetXVals() const { return xVals_; }
    const std::vector<double>& GetYVals() const { return yVals_; }
    int GetIndex() const { return index_; }
    
    double GetMinX() const override {
        if (xVals_.empty()) return 0;
        return *std::min_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMaxX() const override {
        if (xVals_.empty()) return 1;
        return *std::max_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMinY() const override {
        if (yVals_.empty()) return 0;
        return *std::min_element(yVals_.begin(), yVals_.end());
    }
    
    double GetMaxY() const override {
        if (yVals_.empty()) return 1;
        return *std::max_element(yVals_.begin(), yVals_.end());
    }
    
    int GetNumPoints() const override {
        return static_cast<int>(xVals_.size());
    }
    
    std::string GetTitle() const override {
        return title_;
    }
    
    void Draw(GraphWidget* widget, const CoordSystemParams& params) override;
};

// Multi-function data
class MultiLoadedFunction : public LoadedFunction {
private:
    std::string title_;
    std::vector<std::string> legend_;
    std::vector<double> xVals_;
    std::vector<std::vector<double>> yVals_;  // Each inner vector is one function
    std::vector<FunctionDrawStyle> functionStyles_;  // Individual styles for each sub-function
    std::vector<bool> functionVisibility_;  // Individual visibility for each sub-function
    
public:
    MultiLoadedFunction(const std::string& title, const std::vector<std::string>& legend)
        : title_(title), legend_(legend) {
        yVals_.resize(legend.size());
        functionStyles_.resize(legend.size());
        functionVisibility_.resize(legend.size(), true);
    }
    
    void AddPoint(double x, const std::vector<double>& yValues) {
        xVals_.push_back(x);
        for (size_t i = 0; i < yValues.size() && i < yVals_.size(); ++i) {
            yVals_[i].push_back(yValues[i]);
        }
    }
    
    const std::vector<double>& GetXVals() const { return xVals_; }
    const std::vector<std::vector<double>>& GetYVals() const { return yVals_; }
    
    int GetDimension() const override {
        return static_cast<int>(yVals_.size());
    }
    
    std::string GetFunctionTitle(int index) const override {
        if (index >= 0 && index < static_cast<int>(legend_.size())) {
            return legend_[index];
        }
        return "Function " + std::to_string(index);
    }
    
    // Per-function visibility control
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
    
    // Get/set style for specific sub-function
    FunctionDrawStyle& GetFunctionStyle(int index) {
        return functionStyles_[index];
    }
    
    const FunctionDrawStyle& GetFunctionStyle(int index) const {
        return functionStyles_[index];
    }
    
    void SetFunctionStyle(int index, const FunctionDrawStyle& style) {
        if (index >= 0 && index < static_cast<int>(functionStyles_.size())) {
            functionStyles_[index] = style;
        }
    }
    
    double GetMinX() const override {
        if (xVals_.empty()) return 0;
        return *std::min_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMaxX() const override {
        if (xVals_.empty()) return 1;
        return *std::max_element(xVals_.begin(), xVals_.end());
    }
    
    double GetMinY() const override {
        double minVal = 0;
        bool first = true;
        for (size_t i = 0; i < yVals_.size(); ++i) {
            // Only consider visible functions
            if (!functionVisibility_[i]) continue;
            
            const auto& func = yVals_[i];
            if (!func.empty()) {
                double funcMin = *std::min_element(func.begin(), func.end());
                if (first || funcMin < minVal) {
                    minVal = funcMin;
                    first = false;
                }
            }
        }
        return minVal;
    }
    
    double GetMaxY() const override {
        double maxVal = 1;
        bool first = true;
        for (size_t i = 0; i < yVals_.size(); ++i) {
            // Only consider visible functions
            if (!functionVisibility_[i]) continue;
            
            const auto& func = yVals_[i];
            if (!func.empty()) {
                double funcMax = *std::max_element(func.begin(), func.end());
                if (first || funcMax > maxVal) {
                    maxVal = funcMax;
                    first = false;
                }
            }
        }
        return maxVal;
    }
    
    int GetNumPoints() const override {
        return static_cast<int>(xVals_.size());
    }
    
    std::string GetTitle() const override {
        return title_;
    }
    
    void Draw(GraphWidget* widget, const CoordSystemParams& params) override;
};

#endif // MML_DATA_H
