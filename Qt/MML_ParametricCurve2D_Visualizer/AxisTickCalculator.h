#ifndef AXIS_TICK_CALCULATOR_H
#define AXIS_TICK_CALCULATOR_H

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdio>

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
    static AxisTickInfo CalculateTicks(double dataMin, double dataMax, int targetTickCount = 8) {
        AxisTickInfo result;
        
        // Handle edge cases
        if (std::isnan(dataMin) || std::isnan(dataMax) || 
            std::isinf(dataMin) || std::isinf(dataMax)) {
            return CreateDefaultTicks(-10, 10, targetTickCount);
        }
        
        // Handle case where min equals max
        if (std::abs(dataMax - dataMin) < 1e-10) {
            double padding = std::abs(dataMin) * 0.1;
            if (padding < 1e-10) padding = 1.0;
            dataMin -= padding;
            dataMax += padding;
        }
        
        // Ensure min < max
        if (dataMin > dataMax) {
            std::swap(dataMin, dataMax);
        }
        
        double range = dataMax - dataMin;
        double roughTickSpacing = range / (targetTickCount - 1);
        
        // Find magnitude (power of 10)
        double magnitude = std::pow(10, std::floor(std::log10(roughTickSpacing)));
        
        // Normalize to 1-10 range
        double normalizedSpacing = roughTickSpacing / magnitude;
        
        // Find nearest nice number
        double niceSpacing = FindNiceNumber(normalizedSpacing);
        double tickSpacing = niceSpacing * magnitude;
        
        // Round min down and max up to tick boundaries
        double niceMin = std::floor(dataMin / tickSpacing) * tickSpacing;
        double niceMax = std::ceil(dataMax / tickSpacing) * tickSpacing;
        
        result.min = niceMin;
        result.max = niceMax;
        result.tickSpacing = tickSpacing;
        result.decimalPlaces = CalculateDecimalPlaces(tickSpacing);
        result.useScientificNotation = ShouldUseScientificNotation(niceMin, niceMax, tickSpacing);
        result.ticks = GenerateTicks(niceMin, niceMax, tickSpacing, 
                                      result.decimalPlaces, result.useScientificNotation);
        
        return result;
    }
    
    static std::pair<AxisTickInfo, AxisTickInfo> CalculateAxisTicks(
        double dataXMin, double dataXMax,
        double dataYMin, double dataYMax,
        int targetXTicks = 10, int targetYTicks = 8) {
        
        AxisTickInfo xTicks = CalculateTicks(dataXMin, dataXMax, targetXTicks);
        AxisTickInfo yTicks = CalculateTicks(dataYMin, dataYMax, targetYTicks);
        
        return std::make_pair(xTicks, yTicks);
    }
    
    static std::string FormatValue(double value, int decimalPlaces, bool useScientific) {
        char buffer[64];
        
        if (useScientific) {
            std::snprintf(buffer, sizeof(buffer), "%.2E", value);
        } else if (decimalPlaces == 0 || std::abs(value - std::round(value)) < 1e-10) {
            std::snprintf(buffer, sizeof(buffer), "%.0f", value);
        } else {
            std::snprintf(buffer, sizeof(buffer), "%.*f", decimalPlaces, value);
        }
        
        return std::string(buffer);
    }

private:
    static constexpr double NiceNumbers[] = { 1.0, 2.0, 2.5, 5.0, 10.0 };
    
    static double FindNiceNumber(double value) {
        for (double nice : NiceNumbers) {
            if (nice >= value * 0.9) {
                return nice;
            }
        }
        return NiceNumbers[4]; // Return 10 as fallback
    }
    
    static int CalculateDecimalPlaces(double tickSpacing) {
        if (tickSpacing >= 1.0) {
            return 0;
        }
        
        double logVal = std::log10(tickSpacing);
        int decimals = static_cast<int>(std::ceil(-logVal));
        return std::max(0, std::min(decimals, 10));
    }
    
    static bool ShouldUseScientificNotation(double min, double max, double tickSpacing) {
        double maxAbs = std::max(std::abs(min), std::abs(max));
        return maxAbs >= 100000 || (maxAbs > 0 && maxAbs < 0.01);
    }
    
    static std::vector<AxisTick> GenerateTicks(double min, double max, double spacing,
                                                int decimalPlaces, bool useScientific) {
        std::vector<AxisTick> ticks;
        double epsilon = spacing * 1e-10;
        
        for (double value = min; value <= max + epsilon; value += spacing) {
            // Clean up floating point errors for values very close to zero
            if (std::abs(value) < epsilon) {
                value = 0.0;
            }
            
            AxisTick tick;
            tick.value = value;
            tick.label = FormatValue(value, decimalPlaces, useScientific);
            tick.isMajor = true;
            ticks.push_back(tick);
        }
        
        return ticks;
    }
    
    static AxisTickInfo CreateDefaultTicks(double min, double max, int targetTickCount) {
        AxisTickInfo result;
        result.min = min;
        result.max = max;
        result.tickSpacing = (max - min) / (targetTickCount - 1);
        result.decimalPlaces = 0;
        result.useScientificNotation = false;
        result.ticks = GenerateTicks(min, max, result.tickSpacing, 0, false);
        return result;
    }
};

#endif // AXIS_TICK_CALCULATOR_H
