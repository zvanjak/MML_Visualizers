#define NOMINMAX
#include "MMLFileParser.h"
#include <iostream>
#include <algorithm>
#include <cctype>

std::unique_ptr<LoadedParametricCurve2D> MMLFileParser::ParseFile(const std::string& filename, int index) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string typeStr;
    std::getline(file, typeStr);
    typeStr = Trim(typeStr);
    
    if (typeStr == "PARAMETRIC_CURVE_CARTESIAN_2D") {
        return ParseParametricCurve2D(file, index);
    } else {
        throw std::runtime_error("Unsupported format: " + typeStr);
    }
}

std::unique_ptr<LoadedParametricCurve2D> MMLFileParser::ParseParametricCurve2D(std::ifstream& file, int index) {
    std::string title;
    std::getline(file, title);
    title = Trim(title);
    
    std::string line;
    
    // Parse t1 (min parameter value)
    std::getline(file, line);
    auto parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid t1 line");
    }
    double t1 = ParseDouble(parts[1]);
    
    // Parse t2 (max parameter value)
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid t2 line");
    }
    double t2 = ParseDouble(parts[1]);
    
    // Parse NumPoints
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid NumPoints line");
    }
    int numPoints = ParseInt(parts[1]);
    
    auto curve = std::make_unique<LoadedParametricCurve2D>(title, index);
    
    // Parse data points (t, x, y)
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        parts = Split(line, ' ');
        if (parts.size() >= 3) {
            double t = ParseDouble(parts[0]);
            double x = ParseDouble(parts[1]);
            double y = ParseDouble(parts[2]);
            curve->AddPoint(t, x, y);
        }
    }
    
    return curve;
}

double MMLFileParser::ParseDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        throw std::runtime_error("Cannot parse double: " + str);
    }
}

int MMLFileParser::ParseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        throw std::runtime_error("Cannot parse int: " + str);
    }
}

std::string MMLFileParser::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> MMLFileParser::Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        token = Trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}
