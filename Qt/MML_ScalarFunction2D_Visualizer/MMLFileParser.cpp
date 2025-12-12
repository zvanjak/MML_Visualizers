#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool MMLFileParser::LoadScalarFunction2D(const std::string& filename, ScalarFunction2DData& outData)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string line;

    // Read format type
    if (!std::getline(file, line)) {
        std::cerr << "Error: Empty file" << std::endl;
        return false;
    }

    if (line != "SCALAR_FUNCTION_CARTESIAN_2D") {
        std::cerr << "Error: Expected SCALAR_FUNCTION_CARTESIAN_2D format, got: " << line << std::endl;
        return false;
    }

    // Read title
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing title" << std::endl;
        return false;
    }
    outData.title = line;

    // Read x1:
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing x1" << std::endl;
        return false;
    }
    sscanf(line.c_str(), "x1: %lf", &outData.xMin);

    // Read x2:
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing x2" << std::endl;
        return false;
    }
    sscanf(line.c_str(), "x2: %lf", &outData.xMax);

    // Read NumPointsX:
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing NumPointsX" << std::endl;
        return false;
    }
    sscanf(line.c_str(), "NumPointsX: %d", &outData.numPointsX);

    // Read y1:
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing y1" << std::endl;
        return false;
    }
    sscanf(line.c_str(), "y1: %lf", &outData.yMin);

    // Read y2: (or sometimes _secDerY:, handle typo in data)
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing y2" << std::endl;
        return false;
    }
    // Handle both "y2:" and "_secDerY:" (typo in example data)
    if (line.find("y2:") != std::string::npos) {
        sscanf(line.c_str(), "y2: %lf", &outData.yMax);
    } else if (line.find("_secDerY:") != std::string::npos) {
        sscanf(line.c_str(), "_secDerY: %lf", &outData.yMax);
    } else {
        std::cerr << "Error: Unexpected y2 format: " << line << std::endl;
        return false;
    }

    // Read NumPointsY:
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing NumPointsY" << std::endl;
        return false;
    }
    sscanf(line.c_str(), "NumPointsY: %d", &outData.numPointsY);

    // Read data points
    outData.values.clear();
    outData.values.reserve(outData.numPointsX * outData.numPointsY);

    int lineNum = 8;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        std::istringstream iss(line);
        double x, y, z;

        if (!(iss >> x >> y >> z)) {
            std::cerr << "Error parsing line " << lineNum << ": " << line << std::endl;
            continue;
        }

        outData.values.push_back(z);
    }

    file.close();

    // Validate data size
    int expectedCount = outData.numPointsX * outData.numPointsY;
    if (outData.values.size() != expectedCount) {
        std::cerr << "Warning: Expected " << expectedCount << " values, got " << outData.values.size() << std::endl;
    }

    // Calculate bounds
    outData.calculateBounds();

    std::cout << "Loaded scalar function: " << outData.title << std::endl;
    std::cout << "Domain: [" << outData.xMin << ", " << outData.xMax << "] x [" 
              << outData.yMin << ", " << outData.yMax << "]" << std::endl;
    std::cout << "Grid: " << outData.numPointsX << " x " << outData.numPointsY 
              << " = " << outData.values.size() << " points" << std::endl;
    std::cout << "Value range: [" << outData.minValue << ", " << outData.maxValue << "]" << std::endl;

    return true;
}
