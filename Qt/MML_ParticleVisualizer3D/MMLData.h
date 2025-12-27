#ifndef MMLDATA_H
#define MMLDATA_H

#define NOMINMAX

#include <vector>
#include <string>
#include <algorithm>
#include <QColor>

struct Point3D
{
    double x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
};

struct Color
{
    float r, g, b, a;
    
    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float _r, float _g, float _b, float _a = 1.0f) 
        : r(_r), g(_g), b(_b), a(_a) {}
    
    static Color FromQColor(const QColor& qc) {
        return Color(qc.redF(), qc.greenF(), qc.blueF(), qc.alphaF());
    }
};

class ParticleData3D
{
public:
    std::string name;
    Color color;
    double size;  // particle size/radius
    std::vector<Point3D> trajectory;  // positions over time
    bool visible;
    
    ParticleData3D(const std::string& _name, const Color& _color, double _size)
        : name(_name), color(_color), size(_size), visible(true) {}
    
    void AddPosition(const Point3D& pos) {
        trajectory.push_back(pos);
    }
    
    Point3D GetPosition(int step) const {
        if (step >= 0 && step < static_cast<int>(trajectory.size()))
            return trajectory[step];
        return Point3D();
    }
    
    int GetNumSteps() const {
        return static_cast<int>(trajectory.size());
    }
};

struct LoadedParticleSimulation3D
{
    std::string title;
    std::vector<ParticleData3D> particles;
    int numSteps;
    double containerWidth, containerHeight, containerDepth;
    
    LoadedParticleSimulation3D() 
        : title("Particle Simulation 3D"), numSteps(0), 
          containerWidth(10.0), containerHeight(10.0), containerDepth(10.0) {}
    
    double GetMaxDimension() const {
        return std::max({containerWidth, containerHeight, containerDepth});
    }
    
    Point3D GetCenter() const {
        return Point3D(containerWidth / 2.0, containerHeight / 2.0, containerDepth / 2.0);
    }
};

// Display modes matching WPF specification
enum class DisplayMode {
    None,           // Axes only
    BoundingBox,    // Semi-transparent bounding box walls
    CoordinatePlanes // Full coordinate planes extending both directions
};

#endif // MMLDATA_H
