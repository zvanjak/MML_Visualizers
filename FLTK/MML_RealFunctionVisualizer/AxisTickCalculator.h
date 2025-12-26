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
 * 
 * The algorithm:
 * 1. Calculate rough tick spacing based on data range and desired tick count
 * 2. Find the magnitude (power of 10) of the spacing
 * 3. Normalize to 1-10 range and find nearest "nice" number (1, 2, 2.5, 5, 10)
 * 4. Round min/max to tick spacing boundaries
 * 5. Generate tick labels with appropriate formatting
 */
class AxisTickCalculator {
public:
    /**
     * Calculates nice tick values for an axis based on data range.
     * 
     * @param dataMin The minimum value in the data
     * @param dataMax The maximum value in the data
     * @param targetTickCount The approximate number of ticks desired (default: 8)
     * @return An AxisTickInfo containing the calculated ticks and bounds
     */
    static AxisTickInfo CalculateTicks(double dataMin, double dataMax, int targetTickCount = 8);
    
    /**
     * Calculates ticks with additional padding around the data range.
     */
    static AxisTickInfo CalculateTicksWithPadding(double dataMin, double dataMax, 
                                                   int targetTickCount = 8, 
                                                   double paddingPercent = 5);
    
    /**
     * Calculates ticks for both X and Y axes simultaneously.
     * Returns a pair of (xTickInfo, yTickInfo)
     */
    static std::pair<AxisTickInfo, AxisTickInfo> CalculateAxisTicks(
        double dataXMin, double dataXMax,
        double dataYMin, double dataYMax,
        int targetXTicks = 10, int targetYTicks = 8);
    
    /**
     * Formats a numeric value for display on axis labels.
     */
    static std::string FormatValue(double value, int decimalPlaces, bool useScientific);

private:
    // Nice numbers for tick intervals (normalized to 1-10 range)
    static constexpr double NiceNumbers[] = { 1.0, 2.0, 2.5, 5.0, 10.0 };
    
    static double FindNiceNumber(double value);
    static int CalculateDecimalPlaces(double tickSpacing);
    static bool ShouldUseScientificNotation(double min, double max, double tickSpacing);
    static std::vector<AxisTick> GenerateTicks(double min, double max, double spacing, 
                                                int decimalPlaces, bool useScientific);
    static AxisTickInfo CreateDefaultTicks(double min, double max, int targetTickCount);
};

#endif // AXIS_TICK_CALCULATOR_H
