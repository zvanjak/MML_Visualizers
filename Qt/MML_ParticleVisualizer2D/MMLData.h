#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>

// Structure for 2D position (from WPF Vector2Cartesian)
struct Vec2D {
    double x, y;
    
    Vec2D() : x(0), y(0) {}
    Vec2D(double x_, double y_) : x(x_), y(y_) {}
};

// Class for particle/ball (from WPF Ball class)
class Ball {
private:
    std::string name_;
    std::string color_;
    double radius_;
    std::vector<Vec2D> positions_;  // Position at each timestep

public:
    Ball(const std::string& name, const std::string& color, double radius) 
        : name_(name), color_(color), radius_(radius) {}
    
    void AddPosition(const Vec2D& pos) {
        positions_.push_back(pos);
    }
    
    const Vec2D& GetPosition(int timestep) const {
        if (timestep < 0 || timestep >= static_cast<int>(positions_.size())) {
            static Vec2D zero;
            return zero;
        }
        return positions_[timestep];
    }
    
    std::string GetName() const { return name_; }
    std::string GetColor() const { return color_; }
    double GetRadius() const { return radius_; }
    int GetNumTimesteps() const { return static_cast<int>(positions_.size()); }
};

// Simulation data structure
struct SimulationData {
    double width;
    double height;
    int numSteps;
    std::vector<Ball> balls;
    
    SimulationData() : width(1000), height(800), numSteps(0) {}
};

#endif // MML_DATA_H
