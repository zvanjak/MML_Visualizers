#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <QColor>

std::string MMLFileParser::Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> MMLFileParser::Split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter))
    {
        std::string trimmed = Trim(token);
        if (!trimmed.empty())
            tokens.push_back(trimmed);
    }
    
    return tokens;
}

Color MMLFileParser::ParseColorName(const std::string& colorName)
{
    std::string lower = colorName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    QColor qc(QString::fromStdString(colorName));
    if (qc.isValid()) {
        return Color::FromQColor(qc);
    }
    
    // Default to white if color not recognized
    return Color(1.0f, 1.0f, 1.0f);
}

double MMLFileParser::ParseDouble(const std::string& str)
{
    try {
        return std::stod(str);
    }
    catch (...) {
        throw std::runtime_error("Failed to parse double: " + str);
    }
}

int MMLFileParser::ParseInt(const std::string& str)
{
    try {
        return std::stoi(str);
    }
    catch (...) {
        throw std::runtime_error("Failed to parse int: " + str);
    }
}

LoadedParticleSimulation3D MMLFileParser::LoadParticleSimulation3D(const std::string& filename)
{
    LoadedParticleSimulation3D simulation;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string line;
    int lineNumber = 0;
    
    // Read header
    std::getline(file, line);
    lineNumber++;
    if (Trim(line) != "PARTICLE_SIMULATION_DATA_3D") {
        throw std::runtime_error("Invalid file format. Expected PARTICLE_SIMULATION_DATA_3D header");
    }
    
    // Read NumBalls line
    std::getline(file, line);
    lineNumber++;
    std::vector<std::string> parts = Split(line, ':');
    if (parts.size() != 2 || Trim(parts[0]) != "NumBalls") {
        throw std::runtime_error("Expected 'NumBalls:' line");
    }
    int numBalls = ParseInt(parts[1]);
    
    // Read ball attributes
    for (int i = 0; i < numBalls; i++) {
        std::getline(file, line);
        lineNumber++;
        parts = Split(line, ' ');
        
        if (parts.size() != 3) {
            throw std::runtime_error("Invalid ball attribute line: " + line);
        }
        
        std::string name = parts[0];
        Color color = ParseColorName(parts[1]);
        double radius = ParseDouble(parts[2]);
        
        simulation.particles.emplace_back(name, color, radius);
    }
    
    // Read NumSteps line
    std::getline(file, line);
    lineNumber++;
    parts = Split(line, ':');
    if (parts.size() != 2 || Trim(parts[0]) != "NumSteps") {
        throw std::runtime_error("Expected 'NumSteps:' line");
    }
    simulation.numSteps = ParseInt(parts[1]);
    
    // Read steps
    for (int step = 0; step < simulation.numSteps; step++) {
        // Read "Step N T" line
        std::getline(file, line);
        lineNumber++;
        parts = Split(line, ' ');
        
        if (parts.size() < 2 || parts[0] != "Step") {
            throw std::runtime_error("Expected 'Step' line at line " + std::to_string(lineNumber));
        }
        
        int stepNum = ParseInt(parts[1]);
        if (stepNum != step) {
            throw std::runtime_error("Step number mismatch at line " + std::to_string(lineNumber));
        }
        
        // Read ball positions for this step
        for (int i = 0; i < numBalls; i++) {
            std::getline(file, line);
            lineNumber++;
            parts = Split(line, ' ');
            
            if (parts.size() != 4) {
                throw std::runtime_error("Invalid position line: " + line);
            }
            
            int ballIndex = ParseInt(parts[0]);
            double x = ParseDouble(parts[1]);
            double y = ParseDouble(parts[2]);
            double z = ParseDouble(parts[3]);
            
            if (ballIndex != i) {
                throw std::runtime_error("Ball index mismatch at line " + std::to_string(lineNumber));
            }
            
            simulation.particles[i].AddPosition(Point3D(x, y, z));
        }
    }
    
    file.close();
    return simulation;
}
