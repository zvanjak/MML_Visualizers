#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

static std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::vector<std::string> Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = Trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::unique_ptr<LoadedRealFunction> MMLFileParser::ParseRealFunction(
    const std::string& filename, 
    const Color& color) {
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    
    // Read first line: data type (REAL_FUNCTION or REAL_FUNCTION_DATA)
    if (!std::getline(file, line)) {
        throw std::runtime_error("Empty file");
    }
    
    line = Trim(line);
    if (line.find("REAL_FUNCTION") == std::string::npos) {
        throw std::runtime_error("Not a REAL_FUNCTION file");
    }

    // Read title
    if (!std::getline(file, line)) {
        throw std::runtime_error("Missing title");
    }
    std::string title = Trim(line);

    // Create function object
    auto func = std::make_unique<LoadedRealFunction>(title, color);

    double xMin = 0, xMax = 1;
    int numPoints = 0;
    
    // Read x1, x2, and NumPoints (format: "x1: value" or "NumPoints: value")
    for (int i = 0; i < 3; i++) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Missing header line");
        }
        line = Trim(line);
        
        auto parts = Split(line, ':');
        if (parts.size() >= 2) {
            std::string key = parts[0];
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            if (key == "x1") {
                xMin = std::stod(parts[1]);
            } else if (key == "x2") {
                xMax = std::stod(parts[1]);
            } else if (key == "numpoints") {
                numPoints = std::stoi(parts[1]);
            }
        }
    }
    
    func->SetXRange(xMin, xMax);

    // Read points
    int pointsRead = 0;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        auto parts = Split(line, ' ');
        if (parts.size() >= 2) {
            double x = std::stod(parts[0]);
            double y = std::stod(parts[1]);
            func->AddPoint(x, y);
            pointsRead++;
        }
    }

    return func;
}
