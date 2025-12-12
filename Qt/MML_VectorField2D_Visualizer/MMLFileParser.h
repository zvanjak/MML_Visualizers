#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <memory>
#include <string>

class MMLFileParser {
public:
    static std::unique_ptr<VectorField2D> ParseFile(const std::string& filename);

private:
    static std::unique_ptr<VectorField2D> ParseVectorField2D(std::ifstream& file);
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static double ParseDouble(const std::string& str);
};

#endif // MML_FILE_PARSER_H
