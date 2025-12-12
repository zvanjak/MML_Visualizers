#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <algorithm>
#include <limits>

// Structure to hold coordinate system parameters (from WPF CoordSystemParams)
struct CoordSystemParams {
    double xMin, xMax;
    double yMin, yMax;
    int numPoints;
    double windowWidth, windowHeight;
    double scaleX, scaleY;
    double centerX, centerY;
    
    CoordSystemParams() : 
        xMin(0), xMax(1), yMin(0), yMax(1), 
        numPoints(0), windowWidth(800), windowHeight(600),
        scaleX(1), scaleY(1), centerX(0), centerY(0) {}
};

// Class for loaded parametric curve 2D (from WPF LoadedParamCurve2D)
class LoadedParamCurve2D {
private:
    std::string title_;
    int index_;
    std::vector<double> tVals_;
    std::vector<double> xVals_;
    std::vector<double> yVals_;

public:
    LoadedParamCurve2D(const std::string& title, int index) 
        : title_(title), index_(index) {}
    
    void AddPoint(double t, double x, double y) {
        tVals_.push_back(t);
        xVals_.push_back(x);
        yVals_.push_back(y);
    }
    
    const std::vector<double>& GetTVals() const { return tVals_; }
    const std::vector<double>& GetXVals() const { return xVals_; }
    const std::vector<double>& GetYVals() const { return yVals_; }
    int GetIndex() const { return index_; }
    std::string GetTitle() const { return title_; }
    
    int GetNumPoints() const {
        return static_cast<int>(xVals_.size());
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
};

#endif // MML_DATA_H
