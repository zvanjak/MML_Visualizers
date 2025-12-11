#ifndef MML_DATA_H
#define MML_DATA_H

#define NOMINMAX
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>

// Structure to represent a 3D point
struct Point3D {
    double x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

// Structure to represent RGB color
struct Color {
    int r, g, b;
    
    Color() : r(0), g(0), b(0) {}
    Color(int r_, int g_, int b_) : r(r_), g(g_), b(b_) {}
};

// Class representing a 3D parametric curve
class LoadedParametricCurve3D {
public:
    LoadedParametricCurve3D(const std::string& name, double t1, double t2)
        : name_(name), t1_(t1), t2_(t2) {}
    
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
    
    // Get point at index
    Point3D GetPoint(size_t i) const {
        if (i >= tVals_.size()) {
            throw std::out_of_range("Point index out of range");
        }
        return Point3D(xVals_[i], yVals_[i], zVals_[i]);
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
};

#endif // MML_DATA_H
