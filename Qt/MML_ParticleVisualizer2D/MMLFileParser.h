#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <string>

class MMLFileParser {
public:
    // Parse PARTICLE_SIMULATION_DATA_2D format file
    static bool ParseFile(const std::string& filename, SimulationData& data, std::string& errorMsg);

private:
    static std::string Trim(const std::string& str);
};

#endif // MML_FILE_PARSER_H
