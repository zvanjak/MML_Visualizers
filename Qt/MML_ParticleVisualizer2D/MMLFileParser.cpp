#include "MMLFileParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::string MMLFileParser::Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool MMLFileParser::ParseFile(const std::string& filename, SimulationData& data, std::string& errorMsg) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        errorMsg = "Cannot open file: " + filename;
        return false;
    }

    std::string line;
    int lineNum = 0;

    try {
        // Line 1: Format identifier
        if (!std::getline(file, line)) {
            errorMsg = "Empty file";
            return false;
        }
        lineNum++;
        line = Trim(line);
        if (line != "PARTICLE_SIMULATION_DATA_2D") {
            errorMsg = "Invalid format. Expected 'PARTICLE_SIMULATION_DATA_2D'";
            return false;
        }

        // Line 2: Width (with or without colon)
        if (!std::getline(file, line)) {
            errorMsg = "Missing Width line";
            return false;
        }
        lineNum++;
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword >> data.width;
        // Remove trailing colon if present
        if (!keyword.empty() && keyword.back() == ':') {
            keyword.pop_back();
        }
        if (keyword != "Width") {
            errorMsg = "Expected 'Width' keyword at line " + std::to_string(lineNum);
            return false;
        }

        // Line 3: Height (with or without colon)
        if (!std::getline(file, line)) {
            errorMsg = "Missing Height line";
            return false;
        }
        lineNum++;
        iss.clear();
        iss.str(line);
        iss >> keyword >> data.height;
        // Remove trailing colon if present
        if (!keyword.empty() && keyword.back() == ':') {
            keyword.pop_back();
        }
        if (keyword != "Height") {
            errorMsg = "Expected 'Height' keyword at line " + std::to_string(lineNum);
            return false;
        }

        // Line 4: NumBalls (with or without colon)
        if (!std::getline(file, line)) {
            errorMsg = "Missing NumBalls line";
            return false;
        }
        lineNum++;
        int numBalls;
        iss.clear();
        iss.str(line);
        iss >> keyword >> numBalls;
        // Remove trailing colon if present
        if (!keyword.empty() && keyword.back() == ':') {
            keyword.pop_back();
        }
        if (keyword != "NumBalls") {
            errorMsg = "Expected 'NumBalls' keyword at line " + std::to_string(lineNum);
            return false;
        }

        // Parse ball definitions
        for (int i = 0; i < numBalls; i++) {
            if (!std::getline(file, line)) {
                errorMsg = "Missing ball definition at line " + std::to_string(lineNum + 1);
                return false;
            }
            lineNum++;
            
            iss.clear();
            iss.str(line);
            std::string name, color;
            double radius;
            iss >> name >> color >> radius;
            
            if (iss.fail()) {
                errorMsg = "Invalid ball definition at line " + std::to_string(lineNum);
                return false;
            }
            
            data.balls.push_back(Ball(name, color, radius));
        }

        // NumSteps (with or without colon)
        if (!std::getline(file, line)) {
            errorMsg = "Missing NumSteps line";
            return false;
        }
        lineNum++;
        iss.clear();
        iss.str(line);
        iss >> keyword >> data.numSteps;
        // Remove trailing colon if present
        if (!keyword.empty() && keyword.back() == ':') {
            keyword.pop_back();
        }
        if (keyword != "NumSteps") {
            errorMsg = "Expected 'NumSteps' keyword at line " + std::to_string(lineNum);
            return false;
        }

        // Parse timesteps
        for (int step = 0; step < data.numSteps; step++) {
            // Step header: "Step <n> <time>"
            if (!std::getline(file, line)) {
                errorMsg = "Missing step header at line " + std::to_string(lineNum + 1);
                return false;
            }
            lineNum++;
            iss.clear();
            iss.str(line);
            std::string stepKeyword;
            int stepNum;
            double time;
            iss >> stepKeyword >> stepNum >> time;
            if (stepKeyword != "Step") {
                errorMsg = "Expected 'Step' keyword at line " + std::to_string(lineNum);
                return false;
            }

            // Read positions for each ball
            for (int ballIdx = 0; ballIdx < numBalls; ballIdx++) {
                if (!std::getline(file, line)) {
                    errorMsg = "Missing position data at line " + std::to_string(lineNum + 1);
                    return false;
                }
                lineNum++;
                
                iss.clear();
                iss.str(line);
                int index;
                double x, y;
                iss >> index >> x >> y;
                
                if (iss.fail() || index != ballIdx) {
                    errorMsg = "Invalid position data at line " + std::to_string(lineNum);
                    return false;
                }
                
                data.balls[ballIdx].AddPosition(Vec2D(x, y));
            }
        }

    } catch (const std::exception& e) {
        errorMsg = "Parse error at line " + std::to_string(lineNum) + ": " + e.what();
        return false;
    }

    file.close();
    return true;
}
