#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>

class MMLFileParser {
public:
    static std::unique_ptr<LoadedFunction> ParseFile(const std::string& filename, int index);
    
private:
    static std::unique_ptr<LoadedFunction> ParseRealFunction(std::ifstream& file, int index);
    static std::unique_ptr<LoadedFunction> ParseMultiRealFunction(std::ifstream& file);
    
    static double ParseDouble(const std::string& str);
    static int ParseInt(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static std::string Trim(const std::string& str);
};

#endif // MML_FILE_PARSER_H
