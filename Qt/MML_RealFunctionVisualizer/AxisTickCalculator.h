#ifndef AXIS_TICK_CALCULATOR_H
#define AXIS_TICK_CALCULATOR_H

#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Structure to hold information about a single tick
struct AxisTick {
    double value;       // The numeric value at this tick
    std::string label;  // The formatted label to display
    
    AxisTick(double v, const std::string& l) : value(v), label(l) {}
};

// Structure to hold all tick information for an axis
struct AxisTickInfo {
    double min;         // Nice minimum value
    double max;         // Nice maximum value
    double step;        // Step between ticks
    std::vector<AxisTick> ticks;    // All tick marks
    int decimalPlaces;  // Decimal places to use for labels
    bool useScientific; // Whether to use scientific notation
};

// Calculator for "nice" axis tick values
// Uses the standard "nice numbers" algorithm (1, 2, 2.5, 5, 10)
class AxisTickCalculator {
public:
    // Calculate nice tick values for both axes
    static std::pair<AxisTickInfo, AxisTickInfo> CalculateAxisTicks(
        double dataXMin, double dataXMax,
        double dataYMin, double dataYMax,
        int targetXTicks = 10,
        int targetYTicks = 8);
    
    // Calculate nice tick values for a single axis
    static AxisTickInfo CalculateTicks(
        double dataMin, double dataMax,
        int targetTicks = 10);
    
    // Format a value for display
    static std::string FormatValue(double value, int decimalPlaces, bool useScientific);
    
private:
    // Find the nearest "nice" number
    static double FindNiceNumber(double range, bool round);
    
    // Nice number sequence: 1, 2, 2.5, 5, 10
    static constexpr double NiceNumbers[] = {1.0, 2.0, 2.5, 5.0, 10.0};
};

// Implementation inline for header-only convenience
inline std::pair<AxisTickInfo, AxisTickInfo> AxisTickCalculator::CalculateAxisTicks(
    double dataXMin, double dataXMax,
    double dataYMin, double dataYMax,
    int targetXTicks,
    int targetYTicks) {
    
    return std::make_pair(
        CalculateTicks(dataXMin, dataXMax, targetXTicks),
        CalculateTicks(dataYMin, dataYMax, targetYTicks)
    );
}

inline AxisTickInfo AxisTickCalculator::CalculateTicks(
    double dataMin, double dataMax,
    int targetTicks) {
    
    AxisTickInfo info;
    
    // Handle degenerate cases
    if (std::abs(dataMax - dataMin) < 1e-10) {
        double center = dataMin;
        double range = std::abs(center) * 0.1;
        if (range < 1e-10) range = 1.0;
        dataMin = center - range;
        dataMax = center + range;
    }
    
    double range = dataMax - dataMin;
    double rawStep = range / targetTicks;
    
    // Find nice step size
    double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
    double normalizedStep = rawStep / magnitude;
    
    double niceStep = 10.0;
    for (double nice : NiceNumbers) {
        if (normalizedStep <= nice) {
            niceStep = nice;
            break;
        }
    }
    
    info.step = niceStep * magnitude;
    
    // Calculate nice min and max
    info.min = std::floor(dataMin / info.step) * info.step;
    info.max = std::ceil(dataMax / info.step) * info.step;
    
    // Ensure bounds include data
    if (info.min > dataMin) info.min -= info.step;
    if (info.max < dataMax) info.max += info.step;
    
    // Determine decimal places and scientific notation
    double absMax = std::max(std::abs(info.min), std::abs(info.max));
    info.useScientific = (absMax >= 1e6 || (absMax < 1e-3 && absMax > 0));
    
    if (info.useScientific) {
        info.decimalPlaces = 2;
    } else {
        int stepMagnitude = static_cast<int>(std::floor(std::log10(std::abs(info.step))));
        info.decimalPlaces = std::max(0, -stepMagnitude);
    }
    
    // Generate tick marks
    double tickValue = info.min;
    while (tickValue <= info.max + info.step * 0.001) {
        // Clean up floating point errors
        if (std::abs(tickValue) < info.step * 0.001) {
            tickValue = 0.0;
        }
        
        std::string label = FormatValue(tickValue, info.decimalPlaces, info.useScientific);
        info.ticks.emplace_back(tickValue, label);
        tickValue += info.step;
    }
    
    return info;
}

inline double AxisTickCalculator::FindNiceNumber(double range, bool round) {
    double exponent = std::floor(std::log10(range));
    double fraction = range / std::pow(10.0, exponent);
    
    double niceFraction;
    if (round) {
        if (fraction < 1.5) niceFraction = 1.0;
        else if (fraction < 3.0) niceFraction = 2.0;
        else if (fraction < 7.0) niceFraction = 5.0;
        else niceFraction = 10.0;
    } else {
        if (fraction <= 1.0) niceFraction = 1.0;
        else if (fraction <= 2.0) niceFraction = 2.0;
        else if (fraction <= 5.0) niceFraction = 5.0;
        else niceFraction = 10.0;
    }
    
    return niceFraction * std::pow(10.0, exponent);
}

inline std::string AxisTickCalculator::FormatValue(double value, int decimalPlaces, bool useScientific) {
    std::ostringstream oss;
    
    if (useScientific) {
        oss << std::scientific << std::setprecision(decimalPlaces) << value;
    } else {
        oss << std::fixed << std::setprecision(decimalPlaces) << value;
        
        // Clean up trailing zeros after decimal point
        std::string result = oss.str();
        if (result.find('.') != std::string::npos) {
            size_t lastNonZero = result.find_last_not_of('0');
            if (result[lastNonZero] == '.') {
                return result.substr(0, lastNonZero);
            }
            return result.substr(0, lastNonZero + 1);
        }
        return result;
    }
    
    return oss.str();
}

#endif // AXIS_TICK_CALCULATOR_H
