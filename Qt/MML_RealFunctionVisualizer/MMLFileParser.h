#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include "MMLData.h"

class MMLFileParser {
public:
    // Main entry point - auto-detects format and returns appropriate type
    static std::unique_ptr<LoadedFunction> ParseFile(const std::string& filename, int index);

private:
    // Format-specific parsers
    static std::unique_ptr<LoadedFunction> ParseRealFunction(std::ifstream& file, int index);
    static std::unique_ptr<LoadedFunction> ParseMultiRealFunction(std::ifstream& file);
    static std::unique_ptr<LoadedFunction> ParseMultiRealFunctionVariableSpaced(std::ifstream& file);
    
    // Utility functions
    static double ParseDouble(const std::string& str);
    static int ParseInt(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static std::string Trim(const std::string& str);
};

#endif // MML_FILE_PARSER_H
