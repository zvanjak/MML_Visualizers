#ifndef MML_FILE_PARSER_H
#define MML_FILE_PARSER_H

#include "MMLData.h"
#include <memory>
#include <string>

// Parse a PARAMETRIC_CURVE_CARTESIAN_3D file
std::unique_ptr<LoadedParametricCurve3D> ParseParametricCurve3D(const std::string& filename);

#endif // MML_FILE_PARSER_H
