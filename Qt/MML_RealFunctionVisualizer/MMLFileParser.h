#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include <string>
#include <memory>
#include "MMLData.h"

class MMLFileParser {
public:
    static std::unique_ptr<LoadedRealFunction> ParseRealFunction(
        const std::string& filename, 
        const Color& color);
};

#endif // MML_FILE_PARSER_H
