#define NOMINMAX
#include "MMLFileParser.h"
#include <iostream>
#include <algorithm>
#include <cctype>

std::unique_ptr<ParticleSimulationData> MMLFileParser::ParseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string typeStr;
    std::getline(file, typeStr);
    typeStr = Trim(typeStr);
    
    if (typeStr == "PARTICLE_SIMULATION_DATA_2D") {
        return ParseParticleSimulation2D(file);
    } else {
        throw std::runtime_error("Unsupported format: " + typeStr);
    }
}

std::unique_ptr<ParticleSimulationData> MMLFileParser::ParseParticleSimulation2D(std::ifstream& file) {
    auto simData = std::make_unique<ParticleSimulationData>();
    
    std::string line;
    
    // Parse NumBalls
    std::getline(file, line);
    auto parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid NumBalls line");
    }
    int numBalls = ParseInt(parts[1]);
    
    // Parse ball definitions (Ball_name color radius)
    for (int i = 0; i < numBalls; ++i) {
        std::getline(file, line);
        parts = Split(line, ' ');
        if (parts.size() < 3) {
            throw std::runtime_error("Invalid ball definition line");
        }
        
        std::string name = parts[0];
        std::string color = parts[1];
        double radius = ParseDouble(parts[2]);
        
        Ball ball(name, color, radius);
        simData->AddBall(ball);
    }
    
    // Parse NumSteps
    std::getline(file, line);
    parts = Split(line, ' ');
    if (parts.size() < 2) {
        throw std::runtime_error("Invalid NumSteps line");
    }
    int numSteps = ParseInt(parts[1]);
    simData->SetNumSteps(numSteps);
    
    // Parse step data
    for (int step = 0; step < numSteps; ++step) {
        // Parse "Step <num> <time>" line
        std::getline(file, line);
        parts = Split(line, ' ');
        if (parts.size() < 3) {
            throw std::runtime_error("Invalid Step line");
        }
        double time = ParseDouble(parts[2]);
        simData->AddTimeStep(time);
        
        // Parse positions for all balls in this step
        for (int ballIdx = 0; ballIdx < numBalls; ++ballIdx) {
            std::getline(file, line);
            parts = Split(line, ' ');
            if (parts.size() < 3) {
                throw std::runtime_error("Invalid position line");
            }
            
            // Format: <ball_index> <x> <y>
            double x = ParseDouble(parts[1]);
            double y = ParseDouble(parts[2]);
            
            simData->GetBall(ballIdx).AddPosition(Vector2D(x, y));
        }
    }
    
    return simData;
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
