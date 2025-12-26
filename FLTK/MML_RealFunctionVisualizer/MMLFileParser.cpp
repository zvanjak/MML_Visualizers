#define NOMINMAX
#include "MMLFileParser.h"
#include <iostream>
#include <algorithm>
#include <cctype>

std::unique_ptr<LoadedFunction> MMLFileParser::ParseFile(const std::string& filename, int index) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string typeStr;
    std::getline(file, typeStr);
    typeStr = Trim(typeStr);
    
    if (typeStr == "REAL_FUNCTION") {
        return ParseRealFunction(file, index);
    } else if (typeStr == "MULTI_REAL_FUNCTION") {
        return ParseMultiRealFunction(file);
    } else if (typeStr == "MULTI_REAL_FUNCTION_VARIABLE_SPACED") {
        return ParseMultiRealFunctionVariableSpaced(file);
    } else if (typeStr == "REAL_FUNCTION_EQUALLY_SPACED") {
        throw std::runtime_error("REAL_FUNCTION_EQUALLY_SPACED not yet supported");
    } else if (typeStr == "REAL_FUNCTION_VARIABLE_SPACED") {
        throw std::runtime_error("REAL_FUNCTION_VARIABLE_SPACED not yet supported");
    } else {
        throw std::runtime_error("Unsupported format: " + typeStr);
    }
}

std::unique_ptr<LoadedFunction> MMLFileParser::ParseRealFunction(std::ifstream& file, int index) {
    std::string title;
    std::getline(file, title);
    title = Trim(title);
    
    std::string line;
    
    // Parse xMin
    std::getline(file, line);
    auto parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid xMin line");
    }
    double xMin = ParseDouble(parts[1]);
    
    // Parse xMax
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid xMax line");
    }
    double xMax = ParseDouble(parts[1]);
    
    // Parse NumPoints
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid NumPoints line");
    }
    int numPoints = ParseInt(parts[1]);
    
    auto func = std::make_unique<SingleLoadedFunction>(title, index);
    
    // Parse data points
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        parts = Split(line, ' ');
        if (parts.size() >= 2) {
            double x = ParseDouble(parts[0]);
            double y = ParseDouble(parts[1]);
            func->AddPoint(x, y);
        }
    }
    
    return func;
}

std::unique_ptr<LoadedFunction> MMLFileParser::ParseMultiRealFunction(std::ifstream& file) {
    std::string title;
    std::getline(file, title);
    title = Trim(title);
    
    std::string line;
    
    // Parse dimension
    std::getline(file, line);
    int dim = ParseInt(Trim(line));
    
    // Parse legend (function names)
    std::vector<std::string> legend;
    for (int i = 0; i < dim; ++i) {
        std::string funcName;
        std::getline(file, funcName);
        legend.push_back(Trim(funcName));
    }
    
    // Parse xMin
    std::getline(file, line);
    auto parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid xMin line");
    }
    double xMin = ParseDouble(parts[1]);
    
    // Parse xMax
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid xMax line");
    }
    double xMax = ParseDouble(parts[1]);
    
    // Parse NumPoints
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid NumPoints line");
    }
    int numPoints = ParseInt(parts[1]);
    
    auto func = std::make_unique<MultiLoadedFunction>(title, legend);
    
    // Parse data points
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        parts = Split(line, ' ');
        if (parts.size() >= dim + 1) {
            double x = ParseDouble(parts[0]);
            std::vector<double> yValues;
            for (int i = 1; i <= dim; ++i) {
                yValues.push_back(ParseDouble(parts[i]));
            }
            func->AddPoint(x, yValues);
        }
    }
    
    return func;
}

std::unique_ptr<LoadedFunction> MMLFileParser::ParseMultiRealFunctionVariableSpaced(std::ifstream& file) {
    // Format:
    // Title
    // Dimension (number of y-values per point)
    // NumPoints
    // StartTime
    // EndTime
    // Data lines: x y1 y2 y3...
    
    std::string title;
    std::getline(file, title);
    title = Trim(title);
    
    std::string line;
    
    // Parse dimension
    std::getline(file, line);
    int dim = ParseInt(Trim(line));
    
    // Parse number of points
    std::getline(file, line);
    int numPoints = ParseInt(Trim(line));
    
    // Parse start time (ignored, we read actual x values)
    std::getline(file, line);
    // double startTime = ParseDouble(Trim(line));
    
    // Parse end time (ignored, we read actual x values)
    std::getline(file, line);
    // double endTime = ParseDouble(Trim(line));
    
    // Create legend with default names
    std::vector<std::string> legend;
    for (int i = 0; i < dim; ++i) {
        legend.push_back(title + " - Function " + std::to_string(i + 1));
    }
    
    auto func = std::make_unique<MultiLoadedFunction>(title, legend);
    
    // Parse data points
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        auto parts = Split(line, ' ');
        if (parts.size() >= static_cast<size_t>(dim + 1)) {
            double x = ParseDouble(parts[0]);
            std::vector<double> yValues;
            for (int i = 1; i <= dim; ++i) {
                yValues.push_back(ParseDouble(parts[i]));
            }
            func->AddPoint(x, yValues);
        }
    }
    
    return func;
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

std::vector<std::string> MMLFileParser::Split(const std::string& str, char delimiter) {
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

std::string MMLFileParser::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
