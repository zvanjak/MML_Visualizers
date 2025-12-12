#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool MMLFileParser::LoadVectorField3D(const std::string& filename, LoadedVectorField3D& outData)
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

    if (line != "VECTOR_FIELD_3D_CARTESIAN") {
        std::cerr << "Error: Expected VECTOR_FIELD_3D_CARTESIAN format, got: " << line << std::endl;
        return false;
    }

    // Read title
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing title" << std::endl;
        return false;
    }
    outData.title = line;

    // Read vector data
    outData.vectors.clear();
    int lineNum = 2;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        std::istringstream iss(line);
        double x, y, z, vx, vy, vz;

        if (!(iss >> x >> y >> z >> vx >> vy >> vz)) {
            std::cerr << "Error parsing line " << lineNum << ": " << line << std::endl;
            continue;
        }

        Vector3D position(x, y, z);
        Vector3D vector(vx, vy, vz);
        outData.vectors.emplace_back(position, vector);
    }

    file.close();

    if (outData.vectors.empty()) {
        std::cerr << "Error: No vectors loaded from file" << std::endl;
        return false;
    }

    // Calculate bounds and magnitude range
    outData.calculateBounds();

    std::cout << "Loaded " << outData.vectors.size() << " vectors" << std::endl;
    std::cout << "Title: " << outData.title << std::endl;
    std::cout << "Magnitude range: [" << outData.minMagnitude << ", " << outData.maxMagnitude << "]" << std::endl;
    std::cout << "Bounds: (" << outData.minBounds.x << ", " << outData.minBounds.y << ", " << outData.minBounds.z << ") to ("
              << outData.maxBounds.x << ", " << outData.maxBounds.y << ", " << outData.maxBounds.z << ")" << std::endl;

    return true;
}
