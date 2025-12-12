#ifndef MMLDATA_H
#define MMLDATA_H

#define NOMINMAX

#include <vector>
#include <string>
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
    double radius;
    std::vector<Point3D> positions;
    
    ParticleData3D(const std::string& _name, const Color& _color, double _radius)
        : name(_name), color(_color), radius(_radius) {}
    
    void AddPosition(const Point3D& pos) {
        positions.push_back(pos);
    }
    
    Point3D GetPosition(int step) const {
        if (step >= 0 && step < static_cast<int>(positions.size()))
            return positions[step];
        return Point3D();
    }
    
    int GetNumSteps() const {
        return static_cast<int>(positions.size());
    }
};

struct LoadedParticleSimulation3D
{
    std::vector<ParticleData3D> particles;
    int numSteps;
    double width, height, depth;
    
    LoadedParticleSimulation3D() 
        : numSteps(0), width(1000), height(1000), depth(1000) {}
};

#endif // MMLDATA_H
