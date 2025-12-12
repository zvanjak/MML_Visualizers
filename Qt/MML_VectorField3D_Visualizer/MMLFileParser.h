#pragma once

#include "MMLData.h"
#include <string>

class MMLFileParser
{
public:
    static bool LoadVectorField3D(const std::string& filename, LoadedVectorField3D& outData);
};
