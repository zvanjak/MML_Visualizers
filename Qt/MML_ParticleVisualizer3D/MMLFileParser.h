#ifndef MMLFILEPARSER_H
#define MMLFILEPARSER_H

#include <string>
#include <vector>
#include "MMLData.h"

class MMLFileParser
{
public:
    static LoadedParticleSimulation3D LoadParticleSimulation3D(const std::string& filename);
    
private:
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static Color ParseColorName(const std::string& colorName);
    static double ParseDouble(const std::string& str);
    static int ParseInt(const std::string& str);
};

#endif // MMLFILEPARSER_H
