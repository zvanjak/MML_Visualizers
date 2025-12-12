#ifndef MML_DATA_H
#define MML_DATA_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <map>

// Structure to hold coordinate system parameters
struct CoordSystemParams {
    double xMin, xMax;
    double yMin, yMax;
    double windowWidth, windowHeight;
    double scaleX, scaleY;
    double centerX, centerY;
    
    CoordSystemParams() : 
        xMin(0), xMax(1), yMin(0), yMax(1), 
        windowWidth(800), windowHeight(600),
        scaleX(1), scaleY(1), centerX(0), centerY(0) {}
};

// Color helper
struct Color {
    unsigned char r, g, b;
    Color(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0) 
        : r(red), g(green), b(blue) {}
        
    static Color FromString(const std::string& colorName);
};

// Structure representing a ball/particle position
struct Vector2D {
    double x, y;
    Vector2D(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
};

// Ball class representing a particle
class Ball {
private:
    std::string name_;
    Color color_;
    double radius_;
    std::vector<Vector2D> positions_;  // Position at each time step

public:
    Ball(const std::string& name, const std::string& colorName, double radius)
        : name_(name), color_(Color::FromString(colorName)), radius_(radius) {}
    
    void AddPosition(const Vector2D& pos) {
        positions_.push_back(pos);
    }
    
    Vector2D GetPosition(int step) const {
        if (step < 0 || step >= static_cast<int>(positions_.size())) {
            throw std::out_of_range("Invalid step index");
        }
        return positions_[step];
    }
    
    const std::string& GetName() const { return name_; }
    Color GetColor() const { return color_; }
    double GetRadius() const { return radius_; }
    int GetNumSteps() const { return static_cast<int>(positions_.size()); }
};

// Particle simulation data
class ParticleSimulationData {
private:
    std::vector<Ball> balls_;
    int numSteps_;
    std::vector<double> timeSteps_;  // Time value for each step
    double width_;   // Simulation space width
    double height_;  // Simulation space height
    
public:
    ParticleSimulationData() : numSteps_(0), width_(800.0), height_(600.0) {}
    
    void AddBall(const Ball& ball) {
        balls_.push_back(ball);
    }
    
    void SetNumSteps(int steps) {
        numSteps_ = steps;
    }
    
    void SetWidth(double width) {
        width_ = width;
    }
    
    void SetHeight(double height) {
        height_ = height;
    }
    
    void AddTimeStep(double time) {
        timeSteps_.push_back(time);
    }
    
    int GetNumBalls() const { return static_cast<int>(balls_.size()); }
    int GetNumSteps() const { return numSteps_; }
    double GetWidth() const { return width_; }
    double GetHeight() const { return height_; }
    const Ball& GetBall(int index) const { return balls_[index]; }
    Ball& GetBall(int index) { return balls_[index]; }
    double GetTimeStep(int step) const { 
        return step < static_cast<int>(timeSteps_.size()) ? timeSteps_[step] : 0.0; 
    }
    
    // Get bounds of simulation space
    void GetBounds(double& xMin, double& xMax, double& yMin, double& yMax) const;
};

#endif // MML_DATA_H
