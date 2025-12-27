#pragma once

#include <vector>
#include <string>

// Simple color structure
struct Color {
    float r, g, b;
    Color(float r_ = 0, float g_ = 0, float b_ = 0) : r(r_), g(g_), b(b_) {}
};

// Get color by index for predefined colors
inline Color GetSurfaceColorByIndex(int index) {
    switch (index) {
        case 0: return Color(1.0f, 0.0f, 0.0f);      // Red (default)
        case 1: return Color(0.0f, 0.0f, 1.0f);      // Blue
        case 2: return Color(0.0f, 0.8f, 0.0f);      // Green
        case 3: return Color(1.0f, 0.65f, 0.0f);     // Orange
        case 4: return Color(0.5f, 0.0f, 0.5f);      // Purple
        case 5: return Color(0.0f, 1.0f, 1.0f);      // Cyan
        case 6: return Color(1.0f, 1.0f, 0.0f);      // Yellow
        case 7: return Color(0.5f, 0.5f, 0.5f);      // Gray
        default: return Color(1.0f, 0.0f, 0.0f);
    }
}

// Get color by index for point markers
inline Color GetPointColorByIndex(int index) {
    switch (index) {
        case 0: return Color(0.0f, 0.0f, 1.0f);      // Blue (default)
        case 1: return Color(1.0f, 0.0f, 0.0f);      // Red
        case 2: return Color(0.0f, 0.8f, 0.0f);      // Green
        case 3: return Color(1.0f, 0.65f, 0.0f);     // Orange
        case 4: return Color(0.5f, 0.0f, 0.5f);      // Purple
        case 5: return Color(0.0f, 1.0f, 1.0f);      // Cyan
        case 6: return Color(0.0f, 0.0f, 0.0f);      // Black
        case 7: return Color(1.0f, 1.0f, 1.0f);      // White
        default: return Color(0.0f, 0.0f, 1.0f);
    }
}

struct ScalarFunction2DData
{
    std::string title;
    
    // Domain parameters
    double xMin, xMax;
    int numPointsX;
    double yMin, yMax;
    int numPointsY;
    
    // Values matrix: values[i * numPointsY + j] = f(x_i, y_j)
    std::vector<double> values;
    
    // Bounds
    double minValue, maxValue;
    
    ScalarFunction2DData() 
        : xMin(0), xMax(0), numPointsX(0)
        , yMin(0), yMax(0), numPointsY(0)
        , minValue(1e10), maxValue(-1e10)
    {}
    
    double getValue(int i, int j) const {
        if (i < 0 || i >= numPointsX || j < 0 || j >= numPointsY) return 0;
        return values[i * numPointsY + j];
    }
    
    void calculateBounds() {
        if (values.empty()) return;
        
        minValue = maxValue = values[0];
        for (double val : values) {
            if (val < minValue) minValue = val;
            if (val > maxValue) maxValue = val;
        }
    }
    
    // Get interpolated X coordinate for grid index i
    double getX(int i) const {
        if (numPointsX <= 1) return xMin;
        return xMin + i * (xMax - xMin) / (numPointsX - 1);
    }
    
    // Get interpolated Y coordinate for grid index j
    double getY(int j) const {
        if (numPointsY <= 1) return yMin;
        return yMin + j * (yMax - yMin) / (numPointsY - 1);
    }
    
    // Get total number of points
    int getTotalPoints() const {
        return numPointsX * numPointsY;
    }
};
