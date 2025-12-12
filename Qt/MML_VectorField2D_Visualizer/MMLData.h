#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <cmath>

// 2D Vector structure
struct Vector2D {
    double x, y;
    
    Vector2D(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    double Magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    double Angle() const {
        return std::atan2(y, x);
    }
    
    Vector2D Normalized() const {
        double mag = Magnitude();
        if (mag > 0) {
            return Vector2D(x / mag, y / mag);
        }
        return Vector2D(0, 0);
    }
};

// Vector field representation - a vector at a position
struct VectorRepr {
    Vector2D position;
    Vector2D vector;
    
    VectorRepr(double px, double py, double vx, double vy)
        : position(px, py), vector(vx, vy) {}
};

// Vector field 2D data
class VectorField2D {
private:
    std::string title_;
    std::vector<VectorRepr> vectors_;
    
public:
    VectorField2D(const std::string& title) : title_(title) {}
    
    void AddVector(double px, double py, double vx, double vy) {
        vectors_.emplace_back(px, py, vx, vy);
    }
    
    const std::vector<VectorRepr>& GetVectors() const { return vectors_; }
    std::string GetTitle() const { return title_; }
    
    int GetNumVectors() const {
        return static_cast<int>(vectors_.size());
    }
    
    void GetBounds(double& xMin, double& xMax, double& yMin, double& yMax) const {
        if (vectors_.empty()) {
            xMin = -1; xMax = 1;
            yMin = -1; yMax = 1;
            return;
        }
        
        xMin = vectors_[0].position.x;
        xMax = vectors_[0].position.x;
        yMin = vectors_[0].position.y;
        yMax = vectors_[0].position.y;
        
        for (const auto& v : vectors_) {
            xMin = std::min(xMin, v.position.x);
            xMax = std::max(xMax, v.position.x);
            yMin = std::min(yMin, v.position.y);
            yMax = std::max(yMax, v.position.y);
        }
    }
    
    double GetMaxMagnitude() const {
        double maxMag = 0;
        for (const auto& v : vectors_) {
            maxMag = std::max(maxMag, v.vector.Magnitude());
        }
        return maxMag;
    }
};

#endif // MML_DATA_H
