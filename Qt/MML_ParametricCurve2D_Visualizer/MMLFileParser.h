#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>

// File parser (based on WPF loading logic)
class MMLFileParser {
public:
    static std::unique_ptr<LoadedParamCurve2D> ParseFile(const std::string& filename, int index);
    
private:
    static std::unique_ptr<LoadedParamCurve2D> ParseParametricCurve2D(std::ifstream& file, int index);
    static double ParseDouble(const std::string& str);
    static int ParseInt(const std::string& str);
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
};

#endif // MML_FILE_PARSER_H
