#ifndef AXIS_TICK_CALCULATOR_H
#define AXIS_TICK_CALCULATOR_H

#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

/**
 * Represents a single tick mark on a coordinate axis.
 */
struct AxisTick {
    double value;          // The numerical value at this tick position
    std::string label;     // The formatted text label to display
    bool isMajor = true;   // Whether this is a major tick
    
    AxisTick(double v = 0, const std::string& l = "", bool major = true) 
        : value(v), label(l), isMajor(major) {}
};

/**
 * Contains comprehensive tick information for rendering an axis.
 */
struct AxisTickInfo {
    double min = 0;              // Nice rounded minimum value
    double max = 1;              // Nice rounded maximum value
    double tickSpacing = 1;      // Spacing between consecutive ticks
    std::vector<AxisTick> ticks; // List of tick marks
    int decimalPlaces = 0;       // Number of decimal places for labels
    bool useScientificNotation = false;
};

/**
 * Calculates optimal axis tick positions with "nice" rounded values.
 */
class AxisTickCalculator {
public:
    static AxisTickInfo CalculateTicks(double dataMin, double dataMax, int targetTickCount = 8);
    static AxisTickInfo CalculateTicksWithPadding(double dataMin, double dataMax, 
                                                   int targetTickCount = 8, 
                                                   double paddingPercent = 5);
    static std::pair<AxisTickInfo, AxisTickInfo> CalculateAxisTicks(
        double dataXMin, double dataXMax,
        double dataYMin, double dataYMax,
        int targetXTicks = 10, int targetYTicks = 8);
    static std::string FormatValue(double value, int decimalPlaces, bool useScientific);

private:
    static constexpr double NiceNumbers[] = { 1.0, 2.0, 2.5, 5.0, 10.0 };
    
    static double FindNiceNumber(double value);
    static int CalculateDecimalPlaces(double tickSpacing);
    static bool ShouldUseScientificNotation(double min, double max, double tickSpacing);
    static std::vector<AxisTick> GenerateTicks(double min, double max, double spacing, 
                                                int decimalPlaces, bool useScientific);
    static AxisTickInfo CreateDefaultTicks(double min, double max, int targetTickCount);
};

#endif // AXIS_TICK_CALCULATOR_H
