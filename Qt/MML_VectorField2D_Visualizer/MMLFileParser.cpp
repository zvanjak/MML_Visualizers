#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

std::unique_ptr<VectorField2D> MMLFileParser::ParseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string typeStr;
    std::getline(file, typeStr);
    typeStr = Trim(typeStr);
    
    if (typeStr == "VECTOR_FIELD_2D_CARTESIAN") {
        return ParseVectorField2D(file);
    } else {
        throw std::runtime_error("Unsupported format: " + typeStr);
    }
}

std::unique_ptr<VectorField2D> MMLFileParser::ParseVectorField2D(std::ifstream& file) {
    std::string title;
    std::getline(file, title);
    title = Trim(title);
    
    auto vectorField = std::make_unique<VectorField2D>(title);
    
    std::string line;
    
    // Parse vector data (px py vx vy)
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        auto parts = Split(line, ' ');
        if (parts.size() >= 4) {
            double px = ParseDouble(parts[0]);
            double py = ParseDouble(parts[1]);
            double vx = ParseDouble(parts[2]);
            double vy = ParseDouble(parts[3]);
            
            vectorField->AddVector(px, py, vx, vy);
        }
    }
    
    return vectorField;
}

std::string MMLFileParser::Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> MMLFileParser::Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

double MMLFileParser::ParseDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        throw std::runtime_error("Failed to parse double: " + str);
    }
}
