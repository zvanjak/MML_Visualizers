#pragma once

#include "MMLData.h"
#include <string>

class MMLFileParser
{
public:
    static bool LoadScalarFunction2D(const std::string& filename, ScalarFunction2DData& outData);
};
