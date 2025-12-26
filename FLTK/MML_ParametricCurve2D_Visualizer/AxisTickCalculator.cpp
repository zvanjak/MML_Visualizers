#include "AxisTickCalculator.h"
#include <limits>
#include <cstdio>

// Define the static constexpr array
constexpr double AxisTickCalculator::NiceNumbers[];

AxisTickInfo AxisTickCalculator::CalculateTicks(double dataMin, double dataMax, int targetTickCount) {
    AxisTickInfo result;
    
    // Handle edge cases: NaN or infinity
    if (std::isnan(dataMin) || std::isnan(dataMax) || 
        std::isinf(dataMin) || std::isinf(dataMax)) {
        return CreateDefaultTicks(-10, 10, targetTickCount);
    }
    
    // Handle case where min equals max
    if (std::abs(dataMax - dataMin) < std::numeric_limits<double>::epsilon()) {
        double padding = std::abs(dataMin) * 0.1;
        if (padding < std::numeric_limits<double>::epsilon()) padding = 1.0;
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

AxisTickInfo AxisTickCalculator::CalculateTicksWithPadding(double dataMin, double dataMax, 
                                                           int targetTickCount, 
                                                           double paddingPercent) {
    double range = dataMax - dataMin;
    double padding = range * paddingPercent / 100.0;
    return CalculateTicks(dataMin - padding, dataMax + padding, targetTickCount);
}

std::pair<AxisTickInfo, AxisTickInfo> AxisTickCalculator::CalculateAxisTicks(
    double dataXMin, double dataXMax,
    double dataYMin, double dataYMax,
    int targetXTicks, int targetYTicks) {
    
    AxisTickInfo xTicks = CalculateTicks(dataXMin, dataXMax, targetXTicks);
    AxisTickInfo yTicks = CalculateTicks(dataYMin, dataYMax, targetYTicks);
    
    return std::make_pair(xTicks, yTicks);
}

double AxisTickCalculator::FindNiceNumber(double value) {
    for (double nice : NiceNumbers) {
        if (nice >= value * 0.9) {
            return nice;
        }
    }
    return NiceNumbers[4]; // Return 10 as fallback
}

int AxisTickCalculator::CalculateDecimalPlaces(double tickSpacing) {
    if (tickSpacing >= 1.0) {
        return 0;
    }
    
    double logVal = std::log10(tickSpacing);
    int decimals = static_cast<int>(std::ceil(-logVal));
    return std::max(0, std::min(decimals, 10));
}

bool AxisTickCalculator::ShouldUseScientificNotation(double min, double max, double tickSpacing) {
    double maxAbs = std::max(std::abs(min), std::abs(max));
    return maxAbs >= 100000 || (maxAbs > 0 && maxAbs < 0.01);
}

std::vector<AxisTick> AxisTickCalculator::GenerateTicks(double min, double max, double spacing,
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

AxisTickInfo AxisTickCalculator::CreateDefaultTicks(double min, double max, int targetTickCount) {
    AxisTickInfo result;
    result.min = min;
    result.max = max;
    result.tickSpacing = (max - min) / (targetTickCount - 1);
    result.decimalPlaces = 0;
    result.useScientificNotation = false;
    result.ticks = GenerateTicks(min, max, result.tickSpacing, 0, false);
    return result;
}

std::string AxisTickCalculator::FormatValue(double value, int decimalPlaces, bool useScientific) {
    char buffer[64];
    
    if (useScientific) {
        // Scientific notation
        std::snprintf(buffer, sizeof(buffer), "%.2E", value);
    } else if (decimalPlaces == 0 || std::abs(value - std::round(value)) < 1e-10) {
        // Integer format
        std::snprintf(buffer, sizeof(buffer), "%.0f", value);
    } else {
        // Fixed decimal format
        std::snprintf(buffer, sizeof(buffer), "%.*f", decimalPlaces, value);
    }
    
    return std::string(buffer);
}
