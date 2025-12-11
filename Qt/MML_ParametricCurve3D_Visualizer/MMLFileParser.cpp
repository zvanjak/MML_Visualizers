#define NOMINMAX
#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::unique_ptr<LoadedParametricCurve3D> ParseParametricCurve3D(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    
    // Read header
    if (!std::getline(file, line) || line.find("PARAMETRIC_CURVE_CARTESIAN_3D") == std::string::npos) {
        throw std::runtime_error("Invalid file format - expected PARAMETRIC_CURVE_CARTESIAN_3D");
    }
    
    // Read curve name
    std::string curveName;
    if (!std::getline(file, curveName)) {
        throw std::runtime_error("Cannot read curve name");
    }
    
    // Read t1
    double t1 = 0.0;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Cannot read t1");
    }
    if (line.find("t1:") != std::string::npos) {
        std::istringstream iss(line.substr(line.find(":") + 1));
        iss >> t1;
    }
    
    // Read t2
    double t2 = 1.0;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Cannot read t2");
    }
    if (line.find("t2:") != std::string::npos) {
        std::istringstream iss(line.substr(line.find(":") + 1));
        iss >> t2;
    }
    
    // Read NumPoints
    int numPoints = 0;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Cannot read NumPoints");
    }
    if (line.find("NumPoints:") != std::string::npos) {
        std::istringstream iss(line.substr(line.find(":") + 1));
        iss >> numPoints;
    }
    
    auto curve = std::make_unique<LoadedParametricCurve3D>(curveName, t1, t2);
    
    // Read data points (t, x, y, z)
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        double t, x, y, z;
        
        if (iss >> t >> x >> y >> z) {
            curve->AddPoint(t, x, y, z);
        }
    }
    
    if (curve->GetNumPoints() == 0) {
        throw std::runtime_error("No data points found in file");
    }
    
    return curve;
}
