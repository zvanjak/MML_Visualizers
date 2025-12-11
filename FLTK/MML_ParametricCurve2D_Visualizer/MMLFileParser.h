#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

class MMLFileParser {
public:
    static std::unique_ptr<LoadedParametricCurve2D> ParseFile(const std::string& filename, int index);
    
private:
    static std::unique_ptr<LoadedParametricCurve2D> ParseParametricCurve2D(std::ifstream& file, int index);
    
    // Helper methods
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static double ParseDouble(const std::string& str);
    static int ParseInt(const std::string& str);
};

#endif // MML_FILE_PARSER_H
